#ifndef MEDIA_VIDEO_DECODER_UNIT_H
#define MEDIA_VIDEO_DECODER_UNIT_H

#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "base/base_type.h"

namespace media {
class VideoDeocderUnit {
public:
VideoDeocderUnit();
~VideoDeocderUnit();
bool initialize(const VideoDecoderConfig& video_decoder_config);
std::shared_ptr<VideoFrame> Decode(
  std::shared_ptr<EncodedAVFrame> encoded_av_frame, int out_width, int out_height);
void ClearBuffer();
private:
void Yuv2Rgb(AVFrame* src_yuv, std::shared_ptr<VideoFrame> video_frame);
bool ConfigDecoder();
bool ConfigSwsContext(int out_width, int out_height);

private:
SwsContext* sws_context_;
AVCodecContext* av_codec_context_;
VideoDecoderConfig video_codec_config_;
AVFrame* av_frame_;
int out_width_;
int out_height_;
};
} // namespace media
#endif