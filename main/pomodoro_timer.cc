#include "pomodoro_timer.h"

#include <cstdio>

void PomodoroTimer::Start() {
    state_ = State::kFocusRunning;
    remaining_seconds_ = kFocusDurationSeconds;
}

bool PomodoroTimer::Pause() {
    if (state_ == State::kFocusRunning) {
        state_ = State::kFocusPaused;
        return true;
    }
    if (state_ == State::kBreakRunning) {
        state_ = State::kBreakPaused;
        return true;
    }
    return false;
}

bool PomodoroTimer::Resume() {
    if (state_ == State::kFocusPaused) {
        state_ = State::kFocusRunning;
        return true;
    }
    if (state_ == State::kBreakPaused) {
        state_ = State::kBreakRunning;
        return true;
    }
    return false;
}

bool PomodoroTimer::Cancel() {
    if (state_ == State::kIdle) {
        return false;
    }
    state_ = State::kIdle;
    remaining_seconds_ = 0;
    return true;
}

PomodoroTimer::TickEvent PomodoroTimer::Tick() {
    if (!IsRunning()) {
        return TickEvent::kNone;
    }

    if (remaining_seconds_ > 0) {
        --remaining_seconds_;
    }

    if (remaining_seconds_ > 0) {
        return TickEvent::kNone;
    }

    if (state_ == State::kFocusRunning) {
        state_ = State::kBreakRunning;
        remaining_seconds_ = kBreakDurationSeconds;
        return TickEvent::kFocusCompleted;
    }

    if (state_ == State::kBreakRunning) {
        state_ = State::kIdle;
        remaining_seconds_ = 0;
        return TickEvent::kBreakCompleted;
    }

    return TickEvent::kNone;
}

bool PomodoroTimer::IsActive() const {
    return state_ != State::kIdle;
}

bool PomodoroTimer::IsRunning() const {
    return state_ == State::kFocusRunning || state_ == State::kBreakRunning;
}

bool PomodoroTimer::IsPaused() const {
    return state_ == State::kFocusPaused || state_ == State::kBreakPaused;
}

bool PomodoroTimer::IsFocusPhase() const {
    return state_ == State::kFocusRunning || state_ == State::kFocusPaused;
}

bool PomodoroTimer::IsBreakPhase() const {
    return state_ == State::kBreakRunning || state_ == State::kBreakPaused;
}

std::string PomodoroTimer::GetRemainingTimeText() const {
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

const char* PomodoroTimer::GetPhaseText() const {
    if (IsFocusPhase()) {
        return "FOCUS";
    }
    if (IsBreakPhase()) {
        return "BREAK";
    }
    return "";
}

const char* PomodoroTimer::GetStatusText() const {
    if (IsRunning()) {
        return "Running";
    }
    if (IsPaused()) {
        return "Paused";
    }
    return "";
}
