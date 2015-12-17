//
//  ffmpeg_common.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/31.
//
//
#include <memory.h>
#include <iostream>
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

VideoCodec AVCodecIDToVideoCodec(AVCodecID av_codec_id) {
  switch (av_codec_id) {
    case AV_CODEC_ID_H264:
      return CODEC_H264;
    case AV_CODEC_ID_MPEG4:
      return CODEC_MPEG4;
    default:
      std::cout << "av_codec_id:" << av_codec_id << std::endl;
      return CODEC_INVALID;
  }
}

AVCodecID VideoCodecToAVCodec(VideoCodec video_codec) {
  switch (video_codec) {
    case CODEC_H264:
      return AV_CODEC_ID_H264;
    case CODEC_MPEG4:
      return AV_CODEC_ID_MPEG4;
    default:
      return AV_CODEC_ID_NONE;
  }
}

VideoPixelFormat AVPixelFormatToVideoPixelFormat(
    AVPixelFormat av_pixel_format) {
  switch (av_pixel_format) {
    case PIX_FMT_YUV422P:
      return YV16;
    case PIX_FMT_YUV444P:
      return YV24;
    case PIX_FMT_YUV420P:
      return YV12;
    case PIX_FMT_YUVJ420P:
      return YV12J;
    case PIX_FMT_YUVA420P:
      return YV12A;
    default:
      std::cout << "AVPixelFormat:" << av_pixel_format << std::endl;
      break;
  }
  return UNKNOWN;
}

AVPixelFormat VideoPixelFormatToAVPixelFormat(
    VideoPixelFormat video_pixel_format) {
  switch (video_pixel_format) {
    case YV16:
      return PIX_FMT_YUV422P;
    case YV24:
      return PIX_FMT_YUV420P;
    case YV12:
      return PIX_FMT_YUVJ420P;
    case YV12J:
      return PIX_FMT_YUVA420P;
    default:
      break;
  }
  return PIX_FMT_YUVA420P;
}

VideoCodecProfile ProfileIDToVideoCodecProfile(int profile) {
  switch (profile) {
    case FF_PROFILE_H264_BASELINE:
      return H264PROFILE_BASELINE;
    case FF_PROFILE_H264_MAIN:
      return H264PROFILE_MAIN;
    case FF_PROFILE_H264_EXTENDED:
      return H264PROFILE_EXTENDED;
    case FF_PROFILE_H264_HIGH:
      return H264PROFILE_HIGH;
    case FF_PROFILE_H264_HIGH_10:
      return H264PROFILE_HIGH10PROFILE;
    case FF_PROFILE_H264_HIGH_422:
      return H264PROFILE_HIGH422PROFILE;
    case FF_PROFILE_H264_HIGH_444_PREDICTIVE:
      return H264PROFILE_HIGH444PREDICTIVEPROFILE;
    default:
      std::cout << "AVCodecProfile:" << profile << std::endl;
      break;
  }
  return VIDEO_CODEC_PROFILE_UNKNOWN;
}

int VideoCodecProfileToProfileID(VideoCodecProfile video_codec_profile) {
  switch (video_codec_profile) {
    case H264PROFILE_BASELINE:
      return FF_PROFILE_H264_BASELINE;
    case H264PROFILE_MAIN:
      return FF_PROFILE_H264_MAIN;
    case H264PROFILE_EXTENDED:
      return FF_PROFILE_H264_EXTENDED;
    case H264PROFILE_HIGH:
      return FF_PROFILE_H264_HIGH;
    case H264PROFILE_HIGH10PROFILE:
      return FF_PROFILE_H264_HIGH_10;
    case H264PROFILE_HIGH422PROFILE:
      return FF_PROFILE_H264_HIGH_422;
    case H264PROFILE_HIGH444PREDICTIVEPROFILE:
      return FF_PROFILE_H264_HIGH_444_PREDICTIVE;
    default:
      break;
  }
  return FF_PROFILE_H264_BASELINE;
}

void AVCodecContextToVideoDecoderConfig(AVCodecContext* av_codec_context,
                                        VideoDecoderConfig* config) {
  config->Initialize(
      AVCodecIDToVideoCodec(av_codec_context->codec_id),
      ProfileIDToVideoCodecProfile(av_codec_context->profile),
      AVPixelFormatToVideoPixelFormat(av_codec_context->pix_fmt),
      av_codec_context->width, av_codec_context->height,
      av_codec_context->coded_width, av_codec_context->coded_height,
      av_codec_context->extradata, av_codec_context->extradata_size);
}

void VideoDecoderConfigToAVCodecContext(VideoDecoderConfig* config,
                                        AVCodecContext* av_codec_context) {
  av_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
  av_codec_context->codec_id = VideoCodecToAVCodec(config->codec_id());
  av_codec_context->profile = VideoCodecProfileToProfileID(config->profile());
  av_codec_context->coded_width = config->width();
  av_codec_context->coded_height = config->height();
  av_codec_context->pix_fmt =
      VideoPixelFormatToAVPixelFormat(config->pixel_format());
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
      AVCodecIDToAudioCodec(audio_codec_context->codec_id),
      AVSampleFormatToSampleFormat(audio_codec_context->sample_fmt),
      FFmpegChannelLayoutToChannelLayout(audio_codec_context->channel_layout),
      audio_codec_context->sample_rate, audio_codec_context->extradata,
      audio_codec_context->extradata_size);
}

void AudioDecoderConfigToAVCodecContext(AudioDecoderConfig* config,
                                        AVCodecContext* audio_codec_context) {
  audio_codec_context->codec_type = AVMEDIA_TYPE_AUDIO;
  audio_codec_context->codec_id = AudioCodecToAVCodecID(config->codec());
  audio_codec_context->sample_fmt =
      SampleFormatToAVSampleFormat(config->sample_format());

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

AudioCodec AVCodecIDToAudioCodec(AVCodecID id) {
  switch (id) {
    case AV_CODEC_ID_AAC:
      return AUDIO_CODEC_AAC;
    case AV_CODEC_ID_MP3:
      return AUDIO_CODEC_MP3;
    case AV_CODEC_ID_AC3:
      return AUDIO_CODEC_AC3;
    default: {
      printf("avcodecid:%x", id);
      AudioCodec result = AUDIO_CODEC_UNKNOWN;
      BOOST_ASSERT(result != AUDIO_CODEC_UNKNOWN);
      return result;
    }
  }
}

AVCodecID AudioCodecToAVCodecID(AudioCodec id) {
  switch (id) {
    case AUDIO_CODEC_AAC:
      return AV_CODEC_ID_AAC;
    case AUDIO_CODEC_MP3:
      return AV_CODEC_ID_MP3;
    default:
      BOOST_ASSERT(0);
      return AV_CODEC_ID_NONE;
  }
}

SampleFormat AVSampleFormatToSampleFormat(AVSampleFormat sample_format) {
  switch (sample_format) {
    case AV_SAMPLE_FMT_U8:
      return SAMPLE_FORMAT_U8;
    case AV_SAMPLE_FMT_S16:
      return SAMPLE_FORMAT_S16;
    case AV_SAMPLE_FMT_S32:
      return SAMPLE_FORMAT_S32;
    case AV_SAMPLE_FMT_S16P:
      return SAMPLE_FORMAT_PLANAR_S16;
    case AV_SAMPLE_FMT_FLTP:
      return SAMPLE_FORMAT_PLANAR_F32;
    case AV_SAMPLE_FMT_S32P:
      return SAMPLE_FORMAT_PLANAR_S32;
    default:
      std::cout << "AVSampleFormat:" << sample_format << std::endl;
      BOOST_ASSERT(0);
      return SAMPLE_FORMAT_UNKNOWN;
  }
}

AVSampleFormat SampleFormatToAVSampleFormat(SampleFormat sample_format) {
  switch (sample_format) {
    case SAMPLE_FORMAT_U8:
      return AV_SAMPLE_FMT_U8;
    case SAMPLE_FORMAT_S16:
      return AV_SAMPLE_FMT_S16;
    case SAMPLE_FORMAT_S32:
      return AV_SAMPLE_FMT_S32;
    case SAMPLE_FORMAT_PLANAR_S16:
      return AV_SAMPLE_FMT_S16P;
    case SAMPLE_FORMAT_PLANAR_F32:
      return AV_SAMPLE_FMT_FLTP;
    case SAMPLE_FORMAT_PLANAR_S32:
      return AV_SAMPLE_FMT_S32P;
    default:
      BOOST_ASSERT(0);
      return AV_SAMPLE_FMT_NONE;
  }
}
ChannelLayout FFmpegChannelLayoutToChannelLayout(int64_t channel_layout) {
  switch (channel_layout) {
    case AV_CH_LAYOUT_MONO:
      return CHANNEL_LAYOUT_MONO;
    case AV_CH_LAYOUT_STEREO:
      return CHANNEL_LAYOUT_STEREO;
    case AV_CH_LAYOUT_2_1:
      return CHANNEL_LAYOUT_2_1;
    case AV_CH_LAYOUT_SURROUND:
      return CHANNEL_LAYOUT_SURROUND;
    case AV_CH_LAYOUT_QUAD:
      return CHANNEL_LAYOUT_QUAD;
    case AV_CH_LAYOUT_4POINT0:
      return CHANNEL_LAYOUT_4_0;
    case AV_CH_LAYOUT_2_2:
      return CHANNEL_LAYOUT_2_2;
    default:
      BOOST_ASSERT(0);
      std::cout << "ffmpegChannelLayout:" << channel_layout << std::endl;
      return CHANNEL_LAYOUT_MONO;
  }
}

int64_t ChannelLayoutToFFmpegChannelLayout(ChannelLayout channel_layout) {
  switch (channel_layout) {
    case CHANNEL_LAYOUT_MONO:
      return AV_CH_LAYOUT_MONO;
    case CHANNEL_LAYOUT_STEREO:
      return AV_CH_LAYOUT_STEREO;
    case CHANNEL_LAYOUT_2_1:
      return AV_CH_LAYOUT_2_1;
    case CHANNEL_LAYOUT_SURROUND:
      return AV_CH_LAYOUT_SURROUND;
    case CHANNEL_LAYOUT_QUAD:
      return AV_CH_LAYOUT_QUAD;
    case CHANNEL_LAYOUT_4_0:
      return AV_CH_LAYOUT_4POINT0;
    case CHANNEL_LAYOUT_2_2:
      return AV_CH_LAYOUT_2_2;
    default:
      BOOST_ASSERT(0);
      return AV_CH_LAYOUT_MONO;
  }
}
int SampleFormatToBytesPerChannel(SampleFormat sample_format) {
  switch (sample_format) {
    case SAMPLE_FORMAT_UNKNOWN:
      return 0;
    case SAMPLE_FORMAT_U8:
      return 1;
    case SAMPLE_FORMAT_PLANAR_S16:
    case SAMPLE_FORMAT_S16:
      return 2;
    case SAMPLE_FORMAT_PLANAR_F32:
    case SAMPLE_FORMAT_S32:
    case SAMPLE_FORMAT_PLANAR_S32:
      return 4;
    default:
      BOOST_ASSERT(0);
      return 0;
  }
}

int ChannelLayoutToChannelCount(ChannelLayout channel_layout) {
  switch (channel_layout) {
    case CHANNEL_LAYOUT_NONE:
      return 0;
    case CHANNEL_LAYOUT_MONO:
      return 1;
    case CHANNEL_LAYOUT_STEREO:
      return 2;
    case CHANNEL_LAYOUT_2_1:
      return 3;
    case CHANNEL_LAYOUT_SURROUND:
    case CHANNEL_LAYOUT_QUAD:
    case CHANNEL_LAYOUT_4_0:
    case CHANNEL_LAYOUT_2_2:
      return 4;
    default:
      BOOST_ASSERT(0);
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
  if (!swr_context_) {
    swr_free(&swr_context_);
    swr_context_ = swr_alloc_set_opts(
        NULL, av_codec_context->channel_layout, AV_SAMPLE_FMT_S16,
        av_frame->sample_rate, av_codec_context->channel_layout,
        (enum AVSampleFormat)av_frame->format, av_frame->sample_rate, 0, NULL);
    if (!swr_context_ || swr_init(swr_context_) < 0) {
      std::cout << "create the swrContext failed" << std::endl;
      return;
    }
  }
  if (swr_context_) {
    const uint8_t** in = (const uint8_t**)av_frame->extended_data;
    int outCount = wanted_nb_samples + 256;
    int outSize = av_samples_get_buffer_size(NULL, av_frame->channels, outCount,
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
    int resampledDataSize = resLen * av_frame->channels *
                            av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

    audio_frame.reset(new AudioFrame(
        *out, resampledDataSize,
        TimeBaseToMillionSecond(av_frame->pkt_pts, GetAudioStreamTimeBase())));
    delete[] * out;
  }  // if(_swrCtx)
}

}  // namespace media
