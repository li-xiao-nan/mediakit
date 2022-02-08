//
//  ffmpeg_common.cpp
//  mediacore
//
//  Created by lixiaonan on 15/8/31.
//
//
#include <memory.h>
#include <iostream>
#include <string>
#include "boost/assert.hpp"
#include "ffmpeg_common.h"
#include "media/base/video_decoder_config.h"
#include "media/base/audio_decoder_config.h"

namespace media {

int kMircoSecondPerSecond = 1000000;
int kMillionSecondPerSecond = 1000;
AVRational kMillionTimeBase = {1, kMillionSecondPerSecond};
AVRational kMicroTimeBase = {1, kMircoSecondPerSecond};
AVRational g_video_stream_time_base;
AVRational g_audio_stream_time_base;

void AVCodecContextToVideoDecoderConfig(AVCodecContext* av_codec_context,
                                        VideoDecoderConfig* config) {
  config->Initialize(
      av_codec_context->codec_id,
      av_codec_context->profile,
      av_codec_context->pix_fmt,
      av_codec_context->width, av_codec_context->height,
      av_codec_context->coded_width, av_codec_context->coded_height,
      av_codec_context->extradata, av_codec_context->extradata_size);
}

void VideoDecoderConfigToAVCodecContext(VideoDecoderConfig* config,
                                        AVCodecContext* av_codec_context) {
  av_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
  av_codec_context->codec_id = config->codec_id();
  av_codec_context->profile = config->profile();
  av_codec_context->coded_width = config->width();
  av_codec_context->coded_height = config->height();
  av_codec_context->pix_fmt =
      config->pixel_format();
  av_codec_context->extradata_size = config->extra_data_size();
  av_codec_context->extradata = (uint8_t*)av_malloc(
      av_codec_context->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
  memcpy(av_codec_context->extradata, config->extra_data(),
         config->extra_data_size());
  memset(av_codec_context->extradata + config->extra_data_size(), '\0',
         FF_INPUT_BUFFER_PADDING_SIZE);
}

void AVStreamToVideoDecoderConfig(const AVStream* stream,
                                  VideoDecoderConfig* config) {
  BOOST_ASSERT(stream != 0);
  if (!stream)
    return;
  AVCodecContextToVideoDecoderConfig(stream->codec, config);
}

int64_t ConvertToTimeBase(int64_t timestamp, const AVRational& time_base) {
  return av_rescale_q(timestamp, kMicroTimeBase, time_base);
}

int64_t TimeBaseToMillionSecond(int64_t timestamp,
                                const AVRational& time_base) {
  return av_rescale_q(timestamp, time_base, kMillionTimeBase);
}

/////////////////////////////////audio//////////////////
void AVCodecContextToAudioDecoderConfig(AVCodecContext* audio_codec_context,
                                        AudioDecoderConfig* config) {
  config->Initialize(
      audio_codec_context->codec_id,
      audio_codec_context->sample_fmt,
      audio_codec_context->channel_layout,
      audio_codec_context->sample_rate, audio_codec_context->extradata,
      audio_codec_context->extradata_size);
}

void AudioDecoderConfigToAVCodecContext(AudioDecoderConfig* config,
                                        AVCodecContext* audio_codec_context) {
  audio_codec_context->codec_type = AVMEDIA_TYPE_AUDIO;
  audio_codec_context->codec_id = config->codec();
  audio_codec_context->sample_fmt = config->sample_format();

  audio_codec_context->channels =
      ChannelLayoutToChannelCount(config->channel_layout());
  audio_codec_context->sample_rate = config->sample_rate();

  if (config->extra_data()) {
    audio_codec_context->extradata_size = config->extra_data_size();
    audio_codec_context->extradata = reinterpret_cast<uint8_t*>(
        av_malloc(config->extra_data_size() + FF_INPUT_BUFFER_PADDING_SIZE));
    memcpy(audio_codec_context->extradata, config->extra_data(),
           config->extra_data_size());
    memset(audio_codec_context->extradata + config->extra_data_size(), '\0',
           FF_INPUT_BUFFER_PADDING_SIZE);
  } else {
    audio_codec_context->extradata = NULL;
    audio_codec_context->extradata_size = 0;
  }
}

void AVStreamToAudioDecoderConfig(const AVStream* stream,
                                  AudioDecoderConfig* config) {
  BOOST_ASSERT(stream != 0);
  if (!stream)
    return;
  AVCodecContextToAudioDecoderConfig(stream->codec, config);
}

int SampleFormatToBytesPerChannel(SampleFormat sample_format) {
  switch (sample_format) {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
      return 1;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
      return 2;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
    case AV_SAMPLE_FMT_FLT:
    case  AV_SAMPLE_FMT_FLTP:
      return 4;
    case AV_SAMPLE_FMT_DBLP:
    case AV_SAMPLE_FMT_DBL:
      return 8;
    default:
      std::string error_msg = "unsupported SampleFormat:" + std::to_string(sample_format);
      BOOST_ASSERT_MSG(0, error_msg.c_str());
      return 0;
  }
}

int ChannelLayoutToChannelCount(int64_t channel_layout) {
  switch (channel_layout) {
    case AV_CH_LAYOUT_MONO:
      return 1;
    case AV_CH_LAYOUT_STEREO:
      return 2;
    case AV_CH_LAYOUT_2_1:
      return 3;
    case AV_CH_LAYOUT_SURROUND:
    case AV_CH_LAYOUT_QUAD:
    case AV_CH_LAYOUT_2_2:
      return 4;
    case AV_CH_LAYOUT_3POINT1:
    case AV_CH_LAYOUT_4POINT0:
      return 5;
    case AV_CH_LAYOUT_4POINT1:
    case AV_CH_LAYOUT_5POINT0:
    case AV_CH_LAYOUT_5POINT0_BACK:
    case AV_CH_LAYOUT_6POINT0_FRONT:
      return 6;
    case AV_CH_LAYOUT_5POINT1:
    case AV_CH_LAYOUT_5POINT1_BACK:
    case AV_CH_LAYOUT_6POINT0:
      return 7;
    case AV_CH_LAYOUT_HEXAGONAL:
    case AV_CH_LAYOUT_6POINT1:
    case AV_CH_LAYOUT_6POINT1_BACK:
      return 8;
    default:
      std::string error_message = "unsupported ChannelLayout:" + std::to_string(channel_layout);
      BOOST_ASSERT_MSG(0, error_message.c_str());
      return 0;
  }
}

void AVFrameToVideoFrame(AVFrame* av_frame, VideoFrame* video_frame) {
  unsigned char *s, *d;
  int height, width;
  width = video_frame->_w;
  height = video_frame->_h;
  s = av_frame->data[0];
  if (s) {
    d = video_frame->_yuvData[0];
    for (int i = 0; i < height; i++) {
      memcpy(d, s, video_frame->_yuvStride[0]);
      s = s + av_frame->linesize[0];
      d = d + video_frame->_yuvStride[0];
    }
  }
  int uvHeight = height >> 1;
  if (av_frame->data[1] && av_frame->data[2]) {
    for (int i = 0; i < uvHeight; i++) {
      memcpy(video_frame->_yuvData[1] + i * video_frame->_yuvStride[1],
             av_frame->data[1] + i * av_frame->linesize[1],
             video_frame->_yuvStride[1]);
      memcpy(video_frame->_yuvData[2] + i * video_frame->_yuvStride[2],
             av_frame->data[2] + i * av_frame->linesize[2],
             video_frame->_yuvStride[2]);
    }
  }
}

void SetVideoStreamTimeBase(const AVRational& time_base) {
  g_video_stream_time_base = time_base;
}

AVRational GetVideoStreamTimeBase() {
  return g_video_stream_time_base;
}

void SetAudioStreamTimeBase(const AVRational& time_base) {
  g_audio_stream_time_base = time_base;
}

AVRational GetAudioStreamTimeBase() {
  return g_audio_stream_time_base;
}

void AVFrameToAudioFrame(AVFrame* av_frame,
                         std::shared_ptr<AudioFrame>& audio_frame,
                         AVCodecContext* av_codec_context) {
  //**********************convert the sample format **********/
  static SwrContext* swr_context_ = NULL;
  int wanted_nb_samples = av_frame->nb_samples;
  int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
  int out_ch_count = 2;
  if (!swr_context_) {
    swr_free(&swr_context_);
    // out_ch_layout: SDL音频播放支持单声道/双声道播放，此处统一转换为双声道音频数据
    // out_sample_fmt: SDL 音频最大支持16位数据类型
    swr_context_ = swr_alloc_set_opts(
      NULL,
      out_ch_layout,
      AV_SAMPLE_FMT_S16,
      av_frame->sample_rate,
      av_codec_context->channel_layout,
      (enum AVSampleFormat)av_frame->format,
      av_frame->sample_rate,
      0,
      NULL);
    if (!swr_context_ || swr_init(swr_context_) < 0) {
      std::cout << "create the swrContext failed" << std::endl;
      return;
    }
  }
  if (swr_context_) {
    const uint8_t** in = (const uint8_t**)av_frame->extended_data;
    int outCount = wanted_nb_samples;
    int outSize = av_samples_get_buffer_size(NULL, out_ch_count, outCount,
                                             AV_SAMPLE_FMT_S16, 0);

    uint8_t* tmpPtr = NULL;
    uint8_t** out = &tmpPtr;
    *out = new uint8_t[outSize];  //(uint8_t*)malloc(sizeof(uint8_t)*outSize);
    memset(*out, 0, outSize);
    if (!(*out)) {
      std::cout << __FILE__ << ": " << __LINE__ << "ERROR: alloc buffer failed"
                << std::endl;
      return;
    }
    int resLen =
        swr_convert(swr_context_, out, outCount, in, av_frame->nb_samples);
    if (resLen < 0) {
      std::cout << __FILE__ << ": " << __LINE__ << "convert the frame failed"
                << std::endl;
      return;
    }
    if (resLen == outCount) {
      std::cout << "WARNING: audio buffer is probably to small" << std::endl;
    }
    int resampledDataSize = resLen * out_ch_count *
                            av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

    audio_frame.reset(new AudioFrame(
        *out, resampledDataSize,
        TimeBaseToMillionSecond(av_frame->pkt_pts, GetAudioStreamTimeBase())));
    delete[] * out;
  }  // if(_swrCtx)
}

}  // namespace media
