#ifndef MEDIA_BASE_AUDIO_DECODER_CONFIG_H_
#define MEDIA_BASE_AUDIO_DECODER_CONFIG_H_

#include <vector>
#include "sample_format.h"
#include "channel_layout.h"

namespace media {

enum AudioCodec {
  AUDIO_CODEC_UNKNOWN,
  AUDIO_CODEC_AAC,
  AUDIO_CODEC_MP3,
  AUDIO_CODEC_AC3,
  AUDIO_CODEC_MAX = AUDIO_CODEC_AC3
};

class AudioDecoderConfig {
 public:
  AudioDecoderConfig();
  AudioDecoderConfig(AudioCodec audio_codec,
                     SampleFormat sample_format,
                     ChannelLayout channle_layout,
                     int sample_per_second,
                     uint8_t* extradata,
                     int extradata_size);
  void Initialize(AudioCodec audio_codec,
                  SampleFormat sample_format,
                  ChannelLayout channle_layout,
                  int sample_per_second,
                  uint8_t* extradata,
                  int extradata_size);
  AudioCodec codec();
  SampleFormat sample_format();
  int sample_per_second();
  ChannelLayout channel_layout();
  uint8_t* extra_data();
  int extra_data_size();
  int bytes_per_channel();
  int bytes_per_frame();
  void ShowAudioConfigInfo();

 private:
  const char* AudioCodecToString(AudioCodec audio_codec);
  const char* SampleFormatToString(SampleFormat sample_format);
  const char* ChannelLayoutToString(ChannelLayout channel_layout);

  AudioCodec audio_codec_;
  SampleFormat sample_format_;
  int sample_per_second_;
  ChannelLayout channel_layout_;
  std::vector<uint8_t> extra_data_;
  int bytes_per_channel_;
  int bytes_per_frame_;
};
}  // namespace media

#endif