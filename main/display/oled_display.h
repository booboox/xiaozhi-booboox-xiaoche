#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "lvgl_display.h"
#include "roboeyes_adapter.h"

#include <memory>

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>


class OledDisplay : public LvglDisplay {
private:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    lv_obj_t* top_bar_ = nullptr;
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* content_left_ = nullptr;
    lv_obj_t* content_right_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* side_bar_ = nullptr;
    lv_obj_t *emotion_label_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    lv_obj_t* standby_container_ = nullptr;
    lv_obj_t* standby_time_label_ = nullptr;
    lv_obj_t* standby_date_label_ = nullptr;
    lv_obj_t* pomodoro_container_ = nullptr;
    lv_obj_t* pomodoro_time_label_ = nullptr;
    lv_obj_t* pomodoro_phase_label_ = nullptr;
    lv_obj_t* pomodoro_status_label_ = nullptr;

    // 动画表情相关
    std::unique_ptr<RoboEyesAdapter> roboeyes_adapter_;

    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void SetupUI_128x64();
    void SetupUI_128x32();
    void SetupStandbyClockUI();
    void SetupPomodoroUI();

public:
    OledDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height, bool mirror_x, bool mirror_y);
    ~OledDisplay();

    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void ShowStandbyClock(bool show) override;
    virtual void SetStandbyClock(const char* time_text, const char* date_text) override;
    virtual void ShowPomodoroTimer(bool show) override;
    virtual void SetPomodoroTimer(const char* time_text, const char* phase_text, const char* status_text) override;
    virtual void SetEmotion(const char* emotion) override;
    virtual void SetTheme(Theme* theme) override;

    // 动画表情相关方法
    virtual void SetAnimatedEmotionMode(bool enable) override;
    virtual void SetEmotionDirection(int direction) override;
    virtual void UpdateAnimatedEmotion() override;
};

#endif // OLED_DISPLAY_H
