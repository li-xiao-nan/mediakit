#ifndef MEDIA_BASE_WALL_CLOCK_TIME_SOURCE_IMPL_H
#define MEDIA_BASE_WALL_CLOCK_TIME_SOURCE_IMPL_H

#include "media/base/time_source.h"
#include "base/timer/wall_clock_timer.h"

using MediaCore::WallClockTimer;
namespace media {
class PlaybackClock : public TimeSource {
 public:
  PlaybackClock();
  virtual void StartTicking();
  virtual void StopTicking();
  virtual void SetPlaybackRate(float rate);
  virtual void SetStartTime(int64_t start_time);
  virtual int64_t GetCurrentMediaTime();

 private:
  bool is_ticking_;
  float playback_rate_;
  int64_t current_media_time_;
  WallClockTimer wall_clock_timer_;
};
}  // namespace media
#endif