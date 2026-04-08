#include "reminder_timer.h"

#include <cstdio>

bool ReminderTimer::Start(int minutes, const std::string& task) {
    if (minutes < 1 || task.empty()) {
        return false;
    }

    state_ = State::kCounting;
    remaining_seconds_ = minutes * 60;
    task_text_ = task;
    return true;
}

bool ReminderTimer::Cancel() {
    if (state_ == State::kIdle) {
        return false;
    }
    state_ = State::kIdle;
    remaining_seconds_ = 0;
    task_text_.clear();
    return true;
}

ReminderTimer::TickEvent ReminderTimer::Tick() {
    if (state_ != State::kCounting) {
        return TickEvent::kNone;
    }

    if (remaining_seconds_ > 0) {
        --remaining_seconds_;
    }

    if (remaining_seconds_ <= 0) {
        remaining_seconds_ = 0;
        state_ = State::kDuePending;
        return TickEvent::kReminderDue;
    }

    return TickEvent::kNone;
}

bool ReminderTimer::IsActive() const {
    return state_ != State::kIdle;
}

bool ReminderTimer::IsDuePending() const {
    return state_ == State::kDuePending;
}

std::string ReminderTimer::GetRemainingTimeText() const {
    int seconds = remaining_seconds_;
    if (seconds < 0) {
        seconds = 0;
    }
    int minutes_part = seconds / 60;
    int seconds_part = seconds % 60;
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes_part, seconds_part);
    return std::string(buffer);
}
