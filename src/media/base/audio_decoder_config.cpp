#include <iostream>
#include "audio_decoder_config.h"
#include "media/ffmpeg/ffmpeg_common.h"

namespace media {
AudioDecoderConfig::AudioDecoderConfig()
    : audio_codec_(AV_CODEC_ID_NONE),
      sample_format_(AV_SAMPLE_FMT_NONE),
      sample_per_second_(0),
      channel_layout_(0),
      bytes_per_channel_(0),
      bytes_per_frame_(0) {}

AudioDecoderConfig::AudioDecoderConfig(AudioCodec audio_codec,
                                       SampleFormat sample_format,
                                       int64_t channel_layout,
                                       int sample_per_second,
                                       uint8_t* extradata, int extradata_size) {
  Initialize(audio_codec, sample_format, channel_layout, sample_per_second,
             extradata, extradata_size);
}

void AudioDecoderConfig::Initialize(AudioCodec audio_codec,
                                    SampleFormat sample_format,
                                    int64_t channel_layout,
                                    int sample_per_second,
                                    uint8_t* extradata,
                                    int extradata_size) {
  audio_codec_ = audio_codec;
  sample_format_ = sample_format;
  channel_layout_ = channel_layout;
  sample_per_second_ = sample_per_second;
  extra_data_.assign(extradata, extradata + extradata_size);
  bytes_per_channel_ = SampleFormatToBytesPerChannel(sample_format_);
  bytes_per_frame_ =
      bytes_per_channel_ * ChannelLayoutToChannelCount(channel_layout_);
}

int AudioDecoderConfig::channel_count() const {
  return ChannelLayoutToChannelCount(channel_layout_);
}

AudioCodec AudioDecoderConfig::codec() { return audio_codec_; }

SampleFormat AudioDecoderConfig::sample_format() const { return sample_format_; }

int AudioDecoderConfig::sample_rate() const { return sample_per_second_; }

int64_t AudioDecoderConfig::channel_layout() const { return channel_layout_; }

uint8_t* AudioDecoderConfig::extra_data() {
  if (extra_data_.empty())
    return NULL;
  else
    return &extra_data_[0];
}
int AudioDecoderConfig::extra_data_size() { return extra_data_.size(); }

int AudioDecoderConfig::bytes_per_channel() { return bytes_per_channel_; }

int AudioDecoderConfig::bytes_per_frame() { return bytes_per_frame_; }

void AudioDecoderConfig::ShowAudioConfigInfo() {
}
}  // namespace media