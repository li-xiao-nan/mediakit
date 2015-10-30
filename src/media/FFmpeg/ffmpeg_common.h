//
//  ffmpeg_common.h
//  mediacore
//
//  Created by 李楠 on 15/8/31.
//
//

#ifndef MEDIA_FFMPEG_FFMPEG_COMMON_H_
#define MEDIA_FFMPEG_FFMPEG_COMMON_H_

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "media/base/sample_format.h"
#include "media/base/channel_layout.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/video_decoder_config.h"

namespace media {
class AudioDecoderConfig;
class VideoDecoderConfig;

void AVStreamToAudioDecoderConfig(const AVStream* stream,
                                  AudioDecoderConfig* config);
void AVStreamToVideoDecoderConfig(const AVStream* stream,
                                  VideoDecoderConfig* config);
void VideoDecoderConfigToAVCodecContext(VideoDecoderConfig* config, AVCodecContext* av_codec_context);
void AudioDecoderConfigToAVCodecContext(AudioDecoderConfig* config, AVCodecContext* av_codec_context);
VideoCodec AVCodecIDToVideoCodec(AVCodecID av_codec_id);
AVCodecID VideoCodecToAVCodec(VideoCodec video_codec);
//return micromillion second
int64_t ConvertToTimeBase(int64_t timestamp, const AVRational& time_base);
VideoPixelFormat AVPixelFormatToVideoPixelFormat(AVPixelFormat av_pixel_format);
AVPixelFormat VideoPixelFormatToAVPixelFormat(VideoPixelFormat video_pixel_format);
VideoCodecProfile ProfileIDToVideoCodecProfile(int profile);
int VideoCodecProfileToProfileID(VideoCodecProfile video_codec_profile);
void AVCodecContextToVideoDecoderConfig(AVCodecContext* av_codec_context,VideoDecoderConfig* config);
AudioCodec AVCodecIDToAudioCodec(AVCodecID id);
AVCodecID AudioCodecToAVCodecID(AudioCodec id);
SampleFormat AVSampleFormatToSampleFormat(AVSampleFormat sample_format);
AVSampleFormat SampleFormatToAVSampleFormat(SampleFormat sample_format);
ChannelLayout FFmpegChannelLayoutToChannelLayout(int64_t channel_layout);
int64_t ChannelLayoutToFFmpegChannelLayout(ChannelLayout channel_layout);
int SampleFormatToBytesPerChannel(SampleFormat sample_format);
int ChannelLayoutToChannelCount(ChannelLayout channel_layout);

}  // namespace media
#endif
