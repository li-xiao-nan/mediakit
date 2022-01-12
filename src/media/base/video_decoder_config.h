#ifndef MEDIA_BASE_VIDEO_DECODER_CONFIG_H_
#define MEDIA_BASE_VIDEO_DECODER_CONFIG_H_

#include <stdint.h>
#include <vector>

#include "media/base/type_define.h"

namespace media {
class VideoDecoderConfig {
 public:
  VideoDecoderConfig();
  void Initialize(VideoCodec video_codec,
                  VideoCodecProfile video_codec_profile,
                  VideoPixelFormat video_pixel_format,
                  int width,
                  int height,
                  int decoded_width,
                  int decoded_height,
                  uint8_t* extra_data,
                  int extra_data_size);
  VideoCodec codec_id();
  VideoCodecProfile profile();
  VideoPixelFormat pixel_format();
  int width();
  int height();
  uint8_t* extra_data();
  int extra_data_size();
  void ShowVideoConfigInfo();
 private:
  VideoCodec codec_id_;
  VideoCodecProfile profile_;
  VideoPixelFormat video_pixel_format_;
  int width_;
  int height_;
  int decoded_width_;
  int decoded_height_;
  std::vector<uint8_t> extra_data_;
};
}  // namespace media

#endif