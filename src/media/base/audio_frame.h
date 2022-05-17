#ifndef MEDIA_BASE_AUDIO_FRAME_H
#define MEDIA_BASE_AUDIO_FRAME_H

#include <stdint.h>
#include <vector>

namespace media {
class AudioFrame {
 public:
  AudioFrame(unsigned char* data, int data_size, int64_t pts);
  ~AudioFrame();
  int data_size();
  int UnReadDataSize();
  void EnableSpeedData() { use_speed_data_ = true;}
  int SpeedDataSize();
  void SetDataSize(int size);
  void SetSpeedDataSize(int size);
  unsigned char* data();
  unsigned char* SpeedData() { return speed_up_data_;}
  int64_t pts();
  int Read(uint8_t* data, int length);

 private:
   bool use_speed_data_;
   unsigned char* data_;
   unsigned char* speed_up_data_;
   // std::vector<unsigned char> data_;
  int64_t pts_;
  int offset_;
  int data_size_;
  int speed_data_size_;
};
}  // namespace media
#endif