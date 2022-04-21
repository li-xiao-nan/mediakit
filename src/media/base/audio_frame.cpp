
#include "media/base/audio_frame.h"

namespace media {
AudioFrame::AudioFrame(unsigned char* data, int data_size, int64_t pts) {
  pts_ = pts;
  data_size_ = data_size;
  data_ = new unsigned char[data_size_];
  memcpy(data_, data, data_size_);
  offset_ = 0;
}

AudioFrame::~AudioFrame() {
  delete [] data_;
  data_ = nullptr;
}

int AudioFrame::Read(uint8_t* data, int length) {
  int current_size, can_read_size;
  current_size = data_size();
  can_read_size = current_size;

  if (length <= current_size) {
	  can_read_size = length;
  } else {
	  can_read_size = current_size;
  }
  memcpy(data, &data_[offset_], can_read_size);
  offset_ += can_read_size;
  return can_read_size;
}
int AudioFrame::data_size() {
  return data_size_ - offset_;
}

unsigned char* AudioFrame::data() {
  if (!data_)
    return NULL;
  return &data_[0];
}

int64_t AudioFrame::pts() {
  return pts_;
}

}  // namespace media
