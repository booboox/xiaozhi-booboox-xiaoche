#ifndef _REMINDER_TIMER_H_
#define _REMINDER_TIMER_H_

#include <string>

class ReminderTimer {
public:
    enum class State {
        kIdle,
        kCounting,
        kDuePending,
    };

    enum class TickEvent {
        kNone,
        kReminderDue,
    };

    bool Start(int minutes, const std::string& task);
    bool Cancel();
    TickEvent Tick();

    bool IsActive() const;
    bool IsDuePending() const;
    State state() const { return state_; }
    int remaining_seconds() const { return remaining_seconds_; }
    const std::string& task_text() const { return task_text_; }

    std::string GetRemainingTimeText() const;

private:
    State state_ = State::kIdle;
    int remaining_seconds_ = 0;
    std::string task_text_;
};

#endif
