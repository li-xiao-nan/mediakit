#ifndef MEDIA_BASE_AUDIO_DECODER_CONFIG_H_
#define MEDIA_BASE_AUDIO_DECODER_CONFIG_H_

#include <stdint.h>
#include <vector>
#include "media/base/type_define.h"
#include "sample_format.h"
#include "channel_layout.h"

namespace media {
class AudioDecoderConfig {
 public:
  AudioDecoderConfig();
  AudioDecoderConfig(AudioCodec audio_codec, SampleFormat sample_format,
                     int64_t channle_layout, int sample_per_second,
                     uint8_t* extradata, int extradata_size);
  void Initialize(
    AudioCodec audio_codec,
    SampleFormat sample_format,
    int64_t channle_layout,
    int sample_per_second,
    uint8_t* extradata,
    int extradata_size);
  AudioCodec codec();
  SampleFormat sample_format() const;
  int sample_rate() const;
  int64_t channel_layout() const ;
  uint8_t* extra_data();
  int extra_data_size();
  int bytes_per_channel();
  int bytes_per_frame();
  void ShowAudioConfigInfo();
  int channel_count() const;

 private:
  AudioCodec audio_codec_;
  SampleFormat sample_format_;
  int sample_per_second_;
  int64_t channel_layout_;
  std::vector<uint8_t> extra_data_;
  int bytes_per_channel_;
  int bytes_per_frame_;
};
}  // namespace media

#endif