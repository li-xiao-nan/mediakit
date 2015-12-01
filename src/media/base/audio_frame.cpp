
namespace media {
AudioFrame::AudioFrame(unsigned char* data, int data_size, int64_t pts) {
  pts_ = pts;
  data_.assign(data, data + data_size);
}

int AudioFrame::data_size() { return data_.size(); }

unsigned char* AudioFrame::data() {
  if (data_.empty()) return NULL;
  return &data_[0];
}

int64_t AudioFrame::pts() { return pts_; }

}  // namespace media
