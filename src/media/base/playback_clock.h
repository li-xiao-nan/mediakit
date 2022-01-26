#ifndef MEDIA_BASE_WALL_CLOCK_TIME_SOURCE_IMPL_H
#define MEDIA_BASE_WALL_CLOCK_TIME_SOURCE_IMPL_H

#include "media/base/time_source.h"
#include "base/timer/wall_clock_timer.h"

using MediaCore::WallClockTimer;
namespace media {
class PlaybackClock : public TimeSource {
 public:
  PlaybackClock();
  virtual void StartTicking() override;
  virtual void StopTicking() override;
  virtual void SetPlaybackRate(float rate) override;
  virtual void SetStartTime(int64_t start_time) override;
  virtual int64_t GetCurrentMediaTime() override;
  virtual void Pause() override;
  virtual void Resume() override;

 private:
  bool is_ticking_;
  float playback_rate_;
  int64_t current_media_time_;
  WallClockTimer wall_clock_timer_;
};
}  // namespace media
#endif