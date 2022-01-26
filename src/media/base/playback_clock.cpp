#include <iostream>

#include "playback_clock.h"

namespace media {
PlaybackClock::PlaybackClock()
    : is_ticking_(false), playback_rate_(1.0), current_media_time_(0) {
}

void PlaybackClock::StartTicking() {
  wall_clock_timer_.Start();
  is_ticking_ = true;
}

void PlaybackClock::StopTicking() {
  wall_clock_timer_.Stop();
  is_ticking_ = false;
}

void PlaybackClock::SetPlaybackRate(float rate) {
  current_media_time_ += static_cast<int64_t>(wall_clock_timer_.Elapsed() * playback_rate_);
  playback_rate_ = rate;
  wall_clock_timer_.Restart();
}

void PlaybackClock::SetStartTime(int64_t start_time) {
  if (is_ticking_)
    return;
  current_media_time_ = start_time;
}

int64_t PlaybackClock::GetCurrentMediaTime() {
  current_media_time_ = static_cast<int64_t>(wall_clock_timer_.Elapsed() * playback_rate_);
  return current_media_time_;
}

void PlaybackClock::Pause() {
  wall_clock_timer_.Pause();
}

void PlaybackClock::Resume() {
  wall_clock_timer_.Resume();
}

}  // namespace media
