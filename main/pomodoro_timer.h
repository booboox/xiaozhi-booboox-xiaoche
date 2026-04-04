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

    std::string GetRemainingTimeText() const;
    const char* GetPhaseText() const;
    const char* GetStatusText() const;

private:
    State state_ = State::kIdle;
    int remaining_seconds_ = 0;
};

#endif
