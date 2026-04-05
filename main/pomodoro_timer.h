#ifndef _POMODORO_TIMER_H_
#define _POMODORO_TIMER_H_

#include <string>

class PomodoroTimer {
public:
    enum class State {
        kIdle,
        kFocusRunning,
        kFocusPaused,
        kBreakRunning,
        kBreakPaused,
    };

    enum class TickEvent {
        kNone,
        kFocusCompleted,
        kBreakCompleted,
    };

    static constexpr int kFocusDurationSeconds = 25 * 60;
    static constexpr int kBreakDurationSeconds = 5 * 60;

    void SetDurations(int focus_minutes, int break_minutes);
    void Start();
    bool Pause();
    bool Resume();
    bool Cancel();
    TickEvent Tick();

    bool IsActive() const;
    bool IsRunning() const;
    bool IsPaused() const;
    bool IsFocusPhase() const;
    bool IsBreakPhase() const;

    State state() const { return state_; }
    int remaining_seconds() const { return remaining_seconds_; }
    int focus_minutes() const { return focus_duration_seconds_ / 60; }
    int break_minutes() const { return break_duration_seconds_ / 60; }

    std::string GetRemainingTimeText() const;
    const char* GetPhaseText() const;
    const char* GetStatusText() const;

private:
    State state_ = State::kIdle;
    int remaining_seconds_ = 0;
    int focus_duration_seconds_ = kFocusDurationSeconds;
    int break_duration_seconds_ = kBreakDurationSeconds;
};

#endif
