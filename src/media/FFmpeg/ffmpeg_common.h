//
//  ffmpeg_common.h
//  mediacore
//
//  Created by 李楠 on 15/8/31.
//
//

#ifndef MEDIA_FFMPEG_FFMPEG_COMMON_H_
#define MEDIA_FFMPEG_FFMPEG_COMMON_H_

#include <memory>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
}

#include "media/base/sample_format.h"
#include "media/base/channel_layout.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "media/base/audio_frame.h"

namespace media {
class AudioDecoderConfig;
class VideoDecoderConfig;

void SetVideoStreamTimeBase(const AVRational& time_base);
AVRational GetVideoStreamTimeBase();

void SetAudioStreamTimeBase(const AVRational& time_base);
AVRational GetAudioStreamTimeBase();

void AVStreamToAudioDecoderConfig(const AVStream* stream,
                                  AudioDecoderConfig* config);

void AVStreamToVideoDecoderConfig(const AVStream* stream,
                                  VideoDecoderConfig* config);

void VideoDecoderConfigToAVCodecContext(VideoDecoderConfig* config,
                                        AVCodecContext* av_codec_context);

void AudioDecoderConfigToAVCodecContext(AudioDecoderConfig* config,
                                        AVCodecContext* av_codec_context);
// convert million second to fixed time_base
int64_t ConvertToTimeBase(int64_t timestamp, const AVRational& time_base);

// fixed time_base to million second
int64_t TimeBaseToMillionSecond(int64_t timestamp, const AVRational& time_base);

void AVCodecContextToVideoDecoderConfig(AVCodecContext* av_codec_context,
                                        VideoDecoderConfig* config);

int SampleFormatToBytesPerChannel(SampleFormat sample_format);

int ChannelLayoutToChannelCount(int64_t channel_layout);

void AVFrameToVideoFrame(AVFrame* av_frame, VideoFrame* video_frame);
void AVFrameToAudioFrame(AVFrame* av_frame,
                         std::shared_ptr<AudioFrame>& audio_frame,
                         AVCodecContext* av_codec_context);

}  // namespace media
#endif
