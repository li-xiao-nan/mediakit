#include "wall_clock_time_source_impl.h"

namespace media {
WallClockTimeSourceImpl::WallClockTimeSourceImpl()
    : is_ticking_(false), playback_rate_(1.0), current_media_time_(0) {}

void WallClockTimeSourceImpl::StartTicking() {
  wall_clock_timer_.Start();
  is_ticking_ = true;
}

void WallClockTimeSourceImpl::StopTicking() {
  wall_clock_timer_.Stop();
  is_ticking_ = false;
}

void WallClockTimeSourceImpl::SetPlaybackRate(float rate) {
  current_media_time_ += wall_clock_timer_.Elapsed() * playback_rate_;
  playback_rate_ = rate;
  wall_clock_timer_.Restart();
}

void WallClockTimeSourceImpl::SetStartTime(int64_t start_time) {
  if (is_ticking_) return;
  current_media_time_ = start_time;
}

int64_t WallClockTimeSourceImpl::GetCurrentMediaTime() {
  current_media_time_ = wall_clock_timer_.Elapsed() * playback_rate_;
  return current_media_time_;
}

}  // namespace media