#include "SchedulerTimer.h"
#include <sys/time.h>

namespace MediaCore{
    
    SchedulerTimer::SchedulerTimer()
    : start_timestamp_(0L)
    , start_pause_timestamp_(0L)
    , paused_time_internal_(0L)
    , escaped_time_(0L){
        
    }
    
    //time Uint: microsecond
    uint64_t SchedulerTimer::GetTicks(){
        struct timeval tv;
        gettimeofday(&tv, 0);
        uint64_t result = static_cast<uint64_t>(tv.tv_sec) * 1000000L;
        result +=tv.tv_usec;
        
        return static_cast<uint64_t>(result/1000.0);
    }
    
    void SchedulerTimer::Start(){
        start_timestamp_ = GetTicks();
    }
    
    void SchedulerTimer::Pause(){
        start_pause_timestamp_ = GetTicks();
    }
    
    void SchedulerTimer::Resume(){
        paused_time_internal_ += start_pause_timestamp_ - GetTicks();
        start_pause_timestamp_ = 0L;
    }
    
    uint64_t SchedulerTimer::EscapedTime(){
        return escaped_time_;
    }
    
    void SchedulerTimer::Stop(){
        
    }
    
    void SchedulerTimer::Advance(){
        escaped_time_ = GetTicks() - start_timestamp_ - paused_time_internal_;
    }
    
} // namespace MediaCore