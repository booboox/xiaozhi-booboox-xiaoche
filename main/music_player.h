#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include <functional>
#include <mutex>

class MusicPlayer {
public:
    MusicPlayer();
    ~MusicPlayer();

    void Play(const std::string& url, const std::string& title);
    void Stop();
    bool IsPlaying() const { return playing_; }
    std::string GetCurrentTitle() const;
    std::string GetStatus() const;
    int GetPositionMs() const;

    using StatusCallback = std::function<void(const std::string& status, const std::string& title)>;
    void SetStatusCallback(StatusCallback cb) { status_cb_ = cb; }

private:
    static void StreamTask(void* arg);
    void StreamTaskImpl();

    void SetStatus(const std::string& status, const std::string& title = "");

    TaskHandle_t task_ = nullptr;
    bool playing_ = false;
    bool stop_requested_ = false;
    std::string current_url_;
    std::string current_title_;
    std::string status_;
    int64_t total_decoded_samples_ = 0;
    int sample_rate_ = 44100;
    mutable std::mutex mutex_;
    StatusCallback status_cb_;
};

#endif
