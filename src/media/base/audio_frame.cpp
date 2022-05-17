
#include "media/base/audio_frame.h"

namespace media {
AudioFrame::AudioFrame(unsigned char* data, int data_size, int64_t pts) {
  use_speed_data_ = false;
  pts_ = pts;
  data_size_ = data_size;
  data_ = new unsigned char[data_size_];
  speed_up_data_ = new unsigned char[data_size_];
  speed_data_size_ = data_size;
  memcpy(data_, data, data_size_);
  offset_ = 0;
}

AudioFrame::~AudioFrame() {
    delete [] data_;
    data_ = nullptr;
    delete [] speed_up_data_;
    speed_up_data_ = nullptr;
}

int AudioFrame::Read(uint8_t* data, int length) {
  int current_size, can_read_size;
  unsigned char* data_source = nullptr;
  if(use_speed_data_) {
    data_source = SpeedData();
    current_size = SpeedDataSize() - offset_;
  } else {
    data_source = this->data();
    current_size = data_size() - offset_;
  }
  
  can_read_size = current_size;

  if (length <= current_size) {
	  can_read_size = length;
  } else {
	  can_read_size = current_size;
  }
  memcpy(data, &data_source[offset_], can_read_size);
  offset_ += can_read_size;
  return can_read_size;
}

int AudioFrame::data_size() {
    return data_size_;

}

int AudioFrame::SpeedDataSize() {
  return  speed_data_size_;
}

int AudioFrame::UnReadDataSize() {
  int size = 0;
  if (use_speed_data_) {
    size = speed_data_size_ - offset_;
  } else {
    size = data_size_ - offset_;
  }
  return size;
}

void AudioFrame::SetSpeedDataSize(int size) {
  speed_data_size_ = size;
}

void AudioFrame::SetDataSize(int size) {
  data_size_ = size;
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
