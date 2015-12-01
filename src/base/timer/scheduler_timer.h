#ifndef SCHEDULER_TIMER_H
#define SCHEDULER_TIMER_H

#include <stdint.h>

namespace MediaCore{
    class SchedulerTimer{
    public:
        SchedulerTimer();
        void Start();
        void Pause();
        void Resume();
        void Stop();
        void Advance();
        uint64_t EscapedTime();
    private:
        uint64_t GetTicks();
        
        uint64_t start_timestamp_;
        uint64_t start_pause_timestamp_;
        uint64_t paused_time_internal_;
        uint64_t escaped_time_;
    };
}


#endif // SCHEDULER_TIMER_H
