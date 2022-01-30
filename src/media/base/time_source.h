#ifndef MEDIA_BASE_TIME_SOURCE_H
#define MEDIA_BASE_TIME_SOURCE_H

#include <stdint.h>

namespace media {
class TimeSource {
 public:
  virtual ~TimeSource() {}
  virtual void StartTicking() = 0;
  virtual void StopTicking() = 0;
  virtual void SetPlaybackRate(float rate) = 0;
  virtual void SetStartTime(int64_t start_time) = 0;
  virtual int64_t GetCurrentMediaTime() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void Seek(int64_t timestamp_ms) = 0;
};
}  // namespace media
#endif