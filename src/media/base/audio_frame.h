#ifndef MEDIA_BASE_AUDIO_FRAME_H
#define MEDIA_BASE_AUDIO_FRAME_H

#include <stdint.h>
#include <vector>

namespace media {
class AudioFrame {
 public:
  AudioFrame(unsigned char* data, int data_size, int64_t pts);
  int data_size();
  unsigned char* data();
  int64_t pts();
  int Read(uint8_t* data, int length);

 private:
  std::vector<unsigned char> data_;
  int64_t pts_;
  int offset_;
};
}  // namespace media
#endif