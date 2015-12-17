#include <iostream>
#include "audio_decoder_config.h"
#include "media/ffmpeg/ffmpeg_common.h"

namespace media {
AudioDecoderConfig::AudioDecoderConfig()
    : audio_codec_(AUDIO_CODEC_UNKNOWN),
      sample_format_(SAMPLE_FORMAT_UNKNOWN),
      sample_per_second_(0),
      channel_layout_(CHANNEL_LAYOUT_NONE),
      bytes_per_channel_(0),
      bytes_per_frame_(0) {}

AudioDecoderConfig::AudioDecoderConfig(AudioCodec audio_codec,
                                       SampleFormat sample_format,
                                       ChannelLayout channel_layout,
                                       int sample_per_second,
                                       uint8_t* extradata, int extradata_size) {
  Initialize(audio_codec, sample_format, channel_layout, sample_per_second,
             extradata, extradata_size);
}

void AudioDecoderConfig::Initialize(AudioCodec audio_codec,
                                    SampleFormat sample_format,
                                    ChannelLayout channel_layout,
                                    int sample_per_second, uint8_t* extradata,
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

int AudioDecoderConfig::channel_count() {
  return ChannelLayoutToChannelCount(channel_layout_);
}

AudioCodec AudioDecoderConfig::codec() { return audio_codec_; }

SampleFormat AudioDecoderConfig::sample_format() { return sample_format_; }

int AudioDecoderConfig::sample_rate() { return sample_per_second_; }

ChannelLayout AudioDecoderConfig::channel_layout() { return channel_layout_; }

uint8_t* AudioDecoderConfig::extra_data() {
  if (extra_data_.empty())
    return NULL;
  else
    return &extra_data_[0];
}
int AudioDecoderConfig::extra_data_size() { return extra_data_.size(); }

int AudioDecoderConfig::bytes_per_channel() { return bytes_per_channel_; }

int AudioDecoderConfig::bytes_per_frame() { return bytes_per_frame_; }

const char* AudioDecoderConfig::AudioCodecToString(AudioCodec audio_codec) {
  switch (audio_codec) {
    case AUDIO_CODEC_AAC:
      return "AUDIO_CODEC_AAC";
    case AUDIO_CODEC_MP3:
      return "AUDIO_CODEC_MP3";
    case AUDIO_CODEC_AC3:
      return "AUDIO_CODEC_AC3";
    default:
      std::cout << "audioCodec:" << audio_codec << std::endl;
      return "unknown audio codec";
  }
}

const char* AudioDecoderConfig::SampleFormatToString(
    SampleFormat sample_format) {
  switch (sample_format) {
    case SAMPLE_FORMAT_U8:
      return "SAMPLE_FORMAT_U8";
    case SAMPLE_FORMAT_S16:
      return "SAMPLE_FORMAT_S16";
    case SAMPLE_FORMAT_S32:
      return "SAMPLE_FORMAT_S32";
    case SAMPLE_FORMAT_PLANAR_S16:
      return "SAMPLE_FORMAT_PLANAR_S16";
    case SAMPLE_FORMAT_PLANAR_F32:
      return "SAMPLE_FORMAT_PLANAR_F32";
    case SAMPLE_FORMAT_PLANAR_S32:
      return "SAMPLE_FORMAT_PLANAR_S32";
    default:
      return "unkonwn sample format";
  }
}

const char* AudioDecoderConfig::ChannelLayoutToString(
    ChannelLayout channel_layout) {
  switch (channel_layout) {
    case CHANNEL_LAYOUT_MONO:
      return "CHANNEL_LAYOUT_MONO";
    case CHANNEL_LAYOUT_STEREO:
      return "CHANNEL_LAYOUT_STEREO";
    case CHANNEL_LAYOUT_2_1:
      return "CHANNEL_LAYOUT_2_1";

    case CHANNEL_LAYOUT_SURROUND:
      return "CHANNEL_LAYOUT_2_1";
    case CHANNEL_LAYOUT_4_0:
      return "CHANNEL_LAYOUT_4_0";
    case CHANNEL_LAYOUT_2_2:
      return "CHANNEL_LAYOUT_2_2";
    case CHANNEL_LAYOUT_QUAD:
      return "CHANNEL_LAYOUT_QUAD";
    default:
      return "unknown channle layout";
  }
}

void AudioDecoderConfig::ShowAudioConfigInfo() {
  printf("audio:\n");
  printf(
      "    codec:%s, sampleformat:%s, channellayout:%s, samplePerSecond:%d\n",
      AudioCodecToString(audio_codec_), SampleFormatToString(sample_format_),
      ChannelLayoutToString(channel_layout_), sample_per_second_);
}
}  // namespace media