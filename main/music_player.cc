#include "music_player.h"
#include "board.h"
#include "audio_codec.h"
#include "esp_log.h"
#include "esp_http_client.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include <cstring>

static const char* TAG = "MusicPlayer";

#define MP3_BUF_SIZE 16384
#define PCM_FRAME_SAMPLES 2304  // max MP3 frame samples

MusicPlayer::MusicPlayer() {}

MusicPlayer::~MusicPlayer() {
    Stop();
}

void MusicPlayer::SetStatus(const std::string& status, const std::string& title) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = status;
    if (!title.empty()) current_title_ = title;
    ESP_LOGI(TAG, "Status: %s, Title: %s", status_.c_str(), current_title_.c_str());
    if (status_cb_) {
        status_cb_(status_, current_title_);
    }
}

void MusicPlayer::Play(const std::string& url, const std::string& title) {
    Stop();
    std::lock_guard<std::mutex> lock(mutex_);
    current_url_ = url;
    current_title_ = title;
    stop_requested_ = false;
    playing_ = true;
    total_decoded_samples_ = 0;
    sample_rate_ = 44100;
    xTaskCreatePinnedToCore(StreamTask, "music_stream", 8192 * 4, this, 5, &task_, 1);
}

void MusicPlayer::Stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!playing_ && task_ == nullptr) return;
        stop_requested_ = true;
        playing_ = false;
    }
    if (task_) {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (task_) {
            vTaskDelete(task_);
            task_ = nullptr;
        }
    }
    SetStatus("stopped", current_title_);
}

std::string MusicPlayer::GetCurrentTitle() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_title_;
}

std::string MusicPlayer::GetStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

int MusicPlayer::GetPositionMs() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sample_rate_ == 0) return 0;
    return (int)(total_decoded_samples_ * 1000 / sample_rate_);
}

void MusicPlayer::StreamTask(void* arg) {
    auto* player = static_cast<MusicPlayer*>(arg);
    player->StreamTaskImpl();
    player->task_ = nullptr;
    vTaskDelete(nullptr);
}

void MusicPlayer::StreamTaskImpl() {
    SetStatus("connecting", current_title_);

    esp_http_client_config_t config = {};
    config.url = current_url_.c_str();
    config.timeout_ms = 10000;
    config.buffer_size = MP3_BUF_SIZE;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        SetStatus("error: http init failed");
        return;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        SetStatus("error: connect failed");
        esp_http_client_cleanup(client);
        return;
    }

    int64_t content_length = esp_http_client_fetch_headers(client);
    ESP_LOGI(TAG, "Connected, content-length: %lld", content_length);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        total_decoded_samples_ = 0;
        sample_rate_ = 44100;
    }

    auto codec = Board::GetInstance().GetAudioCodec();
    if (!codec) {
        SetStatus("error: no audio codec");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return;
    }

    if (!codec->output_enabled()) {
        codec->EnableOutput(true);
    }

    mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    uint8_t* mp3_buf = (uint8_t*)malloc(MP3_BUF_SIZE);
    if (!mp3_buf) {
        SetStatus("error: oom");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return;
    }

    int mp3_fill = 0;
    int16_t pcm_frame[PCM_FRAME_SAMPLES];
    std::vector<int16_t> pcm_out;
    pcm_out.reserve(PCM_FRAME_SAMPLES);
    SetStatus("playing", current_title_);

    while (true) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_requested_) break;
        }

        int read_len = esp_http_client_read(client, (char*)mp3_buf + mp3_fill, MP3_BUF_SIZE - mp3_fill);
        if (read_len < 0) {
            ESP_LOGE(TAG, "HTTP read error");
            break;
        }

        mp3_fill += read_len;
        if (mp3_fill == 0) break;

        // Decode MP3 frames
        mp3dec_frame_info_t info;
        int samples = mp3dec_decode_frame(&mp3d, mp3_buf, mp3_fill, pcm_frame, &info);
        if (samples > 0) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                total_decoded_samples_ += samples;
                sample_rate_ = info.hz ? info.hz : 44100;
            }

            int out_rate = codec->output_sample_rate();
            int in_rate = info.hz ? info.hz : 44100;

            pcm_out.clear();

            if (in_rate == out_rate && info.channels == 1) {
                pcm_out.assign(pcm_frame, pcm_frame + samples);
            } else {
                int64_t out_samples = ((int64_t)samples * out_rate + in_rate - 1) / in_rate;
                int max_ch_idx = (samples - 1) * info.channels;
                for (int i = 0; i < out_samples; i++) {
                    float pos = (float)i * in_rate / out_rate;
                    int src_idx = (int)pos * info.channels;
                    float frac = pos - (int)pos;
                    if (src_idx > max_ch_idx) src_idx = max_ch_idx;

                    int32_t sum = 0;
                    for (int ch = 0; ch < info.channels; ch++) {
                        sum += pcm_frame[src_idx + ch];
                    }
                    int16_t cur = sum / info.channels;

                    if (frac > 0.001f && src_idx < max_ch_idx) {
                        int32_t nsum = 0;
                        for (int ch = 0; ch < info.channels; ch++) {
                            nsum += pcm_frame[src_idx + info.channels + ch];
                        }
                        int16_t nxt = nsum / info.channels;
                        cur = (int16_t)(cur * (1.0f - frac) + nxt * frac);
                    }
                    pcm_out.push_back(cur);
                }
            }
            if (!pcm_out.empty()) {
                codec->OutputData(pcm_out);
            }
            int remaining = mp3_fill - info.frame_bytes;
            if (remaining > 0) {
                memmove(mp3_buf, mp3_buf + info.frame_bytes, remaining);
            }
            mp3_fill = remaining;
        } else if (info.frame_bytes == 0) {
            // Need more data
            if (mp3_fill >= MP3_BUF_SIZE) mp3_fill = 0; // prevent deadlock
        } else {
            mp3_fill = 0;
        }
    }

    free(mp3_buf);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    SetStatus("finished", current_title_);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        playing_ = false;
    }
}
