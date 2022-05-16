#include "media/video_preview/video_decoder_unit.h"

#include "media/FFmpeg/ffmpeg_common.h"

namespace media {
VideoDeocderUnit::VideoDeocderUnit()
  : sws_context_(nullptr)
  , av_codec_context_(nullptr){}

VideoDeocderUnit::~VideoDeocderUnit() {
  if (sws_context_) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  if (av_codec_context_) {
    avcodec_close(av_codec_context_);
    avcodec_free_context(&av_codec_context_);
    av_codec_context_ = nullptr;
  }
}

bool VideoDeocderUnit::initialize(const VideoDecoderConfig& config) {
  bool result = false;
  video_codec_config_ = config;
  result = ConfigDecoder();
  if(!result){
    return result;
  }
  return true;
}

std::shared_ptr<VideoFrame> VideoDeocderUnit::Decode(
  std::shared_ptr<EncodedAVFrame> encoded_avframe,
  int out_width, int out_height) {
  AUTORUNTIMER("VideoDeocderUnit::Decode");
  if(out_width != out_width_ || out_height != out_height_) {
    out_width_ = out_width;
    out_height_ = out_height;
    ConfigSwsContext(out_width_, out_height_);
  }
  int decode_count;
  int decode_result = 0;
  {
    AUTORUNTIMER("VideoDeocderUnit(avcodec_decode_video2)");
    decode_result = avcodec_decode_video2(
        av_codec_context_, av_frame_, &decode_count, encoded_avframe.get());
  }
  std::shared_ptr<VideoFrame> new_video_frame = nullptr;
  
  if (decode_result < 0) {
    return new_video_frame;
  }

  if (decode_count == 0 && !encoded_avframe.get()) {
    return new_video_frame;
  }

  if (decode_count != 0) {
    AUTORUNTIMER("Yuv2Rgb");
    new_video_frame.reset(new VideoFrame(
      out_width, out_height, VideoFrame::RGB));
    Yuv2Rgb(av_frame_, new_video_frame);
    new_video_frame->timestamp_ =
        TimeBaseToMillionSecond(av_frame_->pkt_pts, GetVideoStreamTimeBase());
    av_frame_unref(av_frame_);
  }
  return new_video_frame;
}

void VideoDeocderUnit::ClearBuffer() {
  if (!av_codec_context_)
    return;
  avcodec_flush_buffers(av_codec_context_);
}

bool VideoDeocderUnit::ConfigDecoder() {
  av_codec_context_ = avcodec_alloc_context3(NULL);
  if (!av_codec_context_)
    return false;
  VideoDecoderConfigToAVCodecContext(&video_codec_config_, av_codec_context_);
  av_codec_context_->thread_count = 2;
  av_codec_context_->thread_type = true ? FF_THREAD_SLICE : FF_THREAD_FRAME;
  av_codec_context_->flags |= CODEC_FLAG_EMU_EDGE;
  av_codec_context_->refcounted_frames = 0;
  AVCodec* codec = avcodec_find_decoder(av_codec_context_->codec_id);
  if (!codec || avcodec_open2(av_codec_context_, codec, NULL) < 0) {
    avcodec_free_context(&av_codec_context_);
    return false;
  }
  av_frame_ = av_frame_alloc();
  return true;
}

bool VideoDeocderUnit::ConfigSwsContext(int out_width, int out_height) {
  if(sws_context_) {
    sws_freeContext(sws_context_);
    sws_context_ = nullptr;
  }
  sws_context_ =
      sws_getContext(av_codec_context_->width, av_codec_context_->height,
        av_codec_context_->pix_fmt, out_width, out_height, AV_PIX_FMT_BGR24,
        SWS_BILINEAR, NULL, NULL, NULL);
  return true;
}

void VideoDeocderUnit::Yuv2Rgb(AVFrame* src_yuv,
                                 std::shared_ptr<VideoFrame> video_frame) {
  uint8_t* dist_data[1] = {video_frame->_data};
  int rgba_stride[1] = {video_frame->stride_};
  int result = sws_scale(sws_context_, src_yuv->data, src_yuv->linesize, 0,
                         src_yuv->height, dist_data, rgba_stride);
  if (result == 0) {
    LOGGING(LOG_LEVEL_ERROR) << "scale operat failed, result:" << result;
  }
  return;
}


} // namespace media