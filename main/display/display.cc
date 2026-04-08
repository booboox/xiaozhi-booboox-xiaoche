#include <esp_log.h>
#include <esp_err.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <font_awesome.h>

#include "display.h"
#include "board.h"
#include "application.h"
#include "audio_codec.h"
#include "settings.h"
#include "assets/lang_config.h"

#define TAG "Display"

Display::Display() {
}

Display::~Display() {
}

void Display::SetStatus(const char* status) {
    ESP_LOGW(TAG, "SetStatus: %s", status);
}

void Display::ShowNotification(const std::string &notification, int duration_ms) {
    ShowNotification(notification.c_str(), duration_ms);
}

void Display::ShowNotification(const char* notification, int duration_ms) {
    ESP_LOGW(TAG, "ShowNotification: %s", notification);
}

void Display::UpdateStatusBar(bool update_all) {
}


void Display::SetEmotion(const char* emotion) {
    ESP_LOGW(TAG, "SetEmotion: %s", emotion);
}

void Display::SetChatMessage(const char* role, const char* content) {
    ESP_LOGW(TAG, "Role:%s", role);
    ESP_LOGW(TAG, "     %s", content);
}

void Display::SetTheme(Theme* theme) {
    current_theme_ = theme;
    Settings settings("display", true);
    settings.SetString("theme", theme->name());
}

void Display::SetPowerSaveMode(bool on) {
    ESP_LOGW(TAG, "SetPowerSaveMode: %d", on);
}

void Display::ShowStandbyClock(bool show) {
    ESP_LOGD(TAG, "ShowStandbyClock: %d", show);
}

void Display::SetStandbyClock(const char* time_text, const char* date_text) {
    ESP_LOGD(TAG, "SetStandbyClock time=%s date=%s",
             time_text ? time_text : "",
             date_text ? date_text : "");
}

void Display::ShowPomodoroTimer(bool show) {
    ESP_LOGD(TAG, "ShowPomodoroTimer: %d", show);
}

void Display::SetPomodoroTimer(const char* time_text, const char* phase_text, const char* status_text) {
    ESP_LOGD(TAG, "SetPomodoroTimer time=%s phase=%s status=%s",
             time_text ? time_text : "",
             phase_text ? phase_text : "",
             status_text ? status_text : "");
}

void Display::ShowReminderTimer(bool show) {
    ESP_LOGD(TAG, "ShowReminderTimer: %d", show);
}

void Display::SetReminderTimer(const char* title_text, const char* time_text, const char* task_text) {
    ESP_LOGD(TAG, "SetReminderTimer title=%s time=%s task=%s",
             title_text ? title_text : "",
             time_text ? time_text : "",
             task_text ? task_text : "");
}

void Display::ShowBlessingPage(bool show) {
    ESP_LOGD(TAG, "ShowBlessingPage: %d", show);
}

void Display::SetBlessingMessage(const char* title_text, const char* message_text) {
    ESP_LOGD(TAG, "SetBlessingMessage title=%s message=%s",
             title_text ? title_text : "",
             message_text ? message_text : "");
}

void Display::SetAnimatedEmotionMode(bool enable) {
    if (animated_emotion_mode_ == enable) {
        return;
    }
    animated_emotion_mode_ = enable;
    ESP_LOGI(TAG, "Animated emotion mode: %s", enable ? "enabled" : "disabled");
}

void Display::SetEmotionDirection(int direction) {
    // 默认实现不做任何事情，由子类实现
    ESP_LOGD(TAG, "SetEmotionDirection: %d", direction);
}

void Display::UpdateAnimatedEmotion() {
    // 默认实现不做任何事情，由子类实现
}
