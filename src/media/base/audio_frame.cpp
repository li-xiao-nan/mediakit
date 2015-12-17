
#include "media/base/audio_frame.h"

namespace media {
AudioFrame::AudioFrame(unsigned char* data, int data_size, int64_t pts) {
  pts_ = pts;
  data_.assign(data, data + data_size);
  offset_ = 0;
}

int AudioFrame::Read(uint8_t* data, int length) {
  int current_size = data_size();
  int read_data_length;
  if (length <= current_size) {
    read_data_length = length;
  } else {
    read_data_length = current_size;
  }
  memcpy(data, &data_[offset_], length);
  offset_ += length;
  return read_data_length;
}
int AudioFrame::data_size() {
  return data_.size() - offset_;
}

unsigned char* AudioFrame::data() {
  if (data_.empty())
    return NULL;
  return &data_[0];
}

int64_t AudioFrame::pts() {
  return pts_;
}

}  // namespace media
