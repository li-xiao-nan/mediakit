#include "ffmpeg_video_decoder.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "media/renderer/renderer_impl.h"
#include "media/base/time_source.h"
#include "log/log_wrapper.h"

namespace media {
FFmpegVideoDecoder::FFmpegVideoDecoder()
    : av_codec_context_(NULL),
      state_(STATE_UNINITIALIZED) {}

FFmpegVideoDecoder::~FFmpegVideoDecoder() { av_frame_free(&av_frame_); }

void FFmpegVideoDecoder::Initialize(const VideoDecoderConfig& config,
                                    InitCB init_cb, OutputCB output_cb) {
  output_cb_ = output_cb;
  config_ = config;
  ConfigureDecoder(true);
  state_ = STATE_NORMAL;
  init_cb(true);
}

void FFmpegVideoDecoder::Decode(
    const std::shared_ptr<EncodedAVFrame> encoded_avframe, DecodeCB decode_cb) {
  // TODO(lixiaonan):
  // Codecs which have the CODEC_CAP_DELAY capability set have a delay
  // between input and output, these need to be fed with avpkt->data=NULL,
  // avpkt->size=0 at the end to return the remaining frames.

  int decode_count;
  // TODO(lixiaonan): log the cost time that decode one frame
  int64_t pre_decode_timestamp = GetPlaybackTime();
  int decode_result = avcodec_decode_video2(
    av_codec_context_, av_frame_, &decode_count, encoded_avframe.get());
  int decode_expend_time = static_cast<int>(GetPlaybackTime() - pre_decode_timestamp);
  if (decode_result < 0) {
    state_ = STATE_OCCUR_ERROR;
    decode_cb(STATUS_DECODE_ERROR);
    return;
  }

  if (decode_count == 0 && !encoded_avframe.get()) {
    state_ = STATE_DECODE_COMPLETED;
    decode_cb(STATUS_DECODE_COMPLETED);
  }

  std::shared_ptr<VideoFrame> new_video_frame;
  if (decode_count != 0) {
    new_video_frame.reset(new VideoFrame(
        av_codec_context_->width, av_codec_context_->height, VideoFrame::YUV));
    AVFrameToVideoFrame(av_frame_, new_video_frame.get());
    Yuv2Rgb(av_frame_, new_video_frame->rgb_format_avframe_);
    new_video_frame->timestamp_ =
        TimeBaseToMillionSecond(av_frame_->pkt_pts, GetVideoStreamTimeBase());
    // record decode cost time
    new_video_frame->_timeRecoder._decodeExpendTime = decode_expend_time;
    new_video_frame->_timeRecoder._addQueueTime = static_cast<int>(GetPlaybackTime());
    new_video_frame->_timeRecoder._pst = new_video_frame->timestamp_;
    av_frame_unref(av_frame_);
    output_cb_(new_video_frame);
    decode_cb(STATUS_OK);
  } else {
    decode_cb(STATUS_DECODE_ABORT);
  }
}

void FFmpegVideoDecoder::Yuv2Rgb(AVFrame* src_yuv, AVFrame* dist_rgb) {
  int width = src_yuv->width;
  int height = src_yuv->height;
  //int dist_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 32);
  //uint8_t* dist_buffer = new uint8_t[dist_buffer_size];
  int result = av_image_fill_arrays(
    dist_rgb->data, dist_rgb->linesize, NULL,
    AV_PIX_FMT_RGBA, width, height, 32);
  if(result < 0) {
    LOGGING(LOG_LEVEL_ERROR) << "initialize dist avframe failed, error:" << result;
    return;
  }
  struct SwsContext* sws =
      sws_getContext(width, height, AVPixelFormat(src_yuv->format),
        width, height, AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);
  result = sws_scale(sws, src_yuv->data, src_yuv->linesize, 0, height, dist_rgb->data, dist_rgb->linesize);
  if(result < 0) {
    LOGGING(LOG_LEVEL_ERROR) << "scale operat failed, result:" << result;
  }

  sws_freeContext(sws);
  return ;
}

void FFmpegVideoDecoder::ReleaseFFmpegResource() {
  avcodec_free_context(&av_codec_context_);
}

bool FFmpegVideoDecoder::ConfigureDecoder(bool low_delay) {
  ReleaseFFmpegResource();
  av_codec_context_ = avcodec_alloc_context3(NULL);
  if (!av_codec_context_) return false;
  VideoDecoderConfigToAVCodecContext(&config_, av_codec_context_);

  av_codec_context_->thread_count = kDecodeThreads;
  av_codec_context_->thread_type =
      low_delay ? FF_THREAD_SLICE : FF_THREAD_FRAME;
  av_codec_context_->flags |= CODEC_FLAG_EMU_EDGE;
  av_codec_context_->refcounted_frames = 0;

  AVCodec* codec = avcodec_find_decoder(av_codec_context_->codec_id);
  if (!codec || avcodec_open2(av_codec_context_, codec, NULL) < 0) {
    ReleaseFFmpegResource();
    return false;
  }
  LOGGING(LOG_LEVEL_INFO) << "video_codec:"
    << avcodec_get_name(av_codec_context_->codec_id)
    << "; profile:" << av_get_profile_name(codec, av_codec_context_->profile)
    << "; pix_fmt:" << av_codec_context_->pix_fmt
    << "; width:" << av_codec_context_->width
    << "; height:" << av_codec_context_->height
    << "; coded_width:" << av_codec_context_->coded_width
    << "; coded_height:" << av_codec_context_->coded_height;
  av_frame_ = av_frame_alloc();
  return true;
}

int64_t FFmpegVideoDecoder::GetPlaybackTime(){
  static std::shared_ptr<TimeSource> playback_clock = NULL;
  if (playback_clock == NULL){
    playback_clock = RendererImpl::GetPlaybackClock(0);
  }

  if (playback_clock == NULL){
    return 0;
  }
  return playback_clock->GetCurrentMediaTime();
}

void FFmpegVideoDecoder::ClearBuffer() {
  if(!av_codec_context_) return;
  avcodec_flush_buffers(av_codec_context_);
}

}  // namespace media