#include "ffmpeg_video_decoder.h"
#include "media/ffmpeg/ffmpeg_common.h"

namespace media {
FFmpegVideoDecoder::FFmpegVideoDecoder(TaskRunner* task_runner)
    : task_runner_(task_runner),
      av_codec_context_(NULL),
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
  if (avcodec_decode_video2(av_codec_context_, av_frame_, &decode_count,
                            encoded_avframe.get()) < 0) {
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
    new_video_frame->timestamp_ =
        TimeBaseToMillionSecond(av_frame_->pkt_pts, GetVideoStreamTimeBase());
    output_cb_(new_video_frame);
    decode_cb(STATUS_OK);
  } else {
    decode_cb(STATUS_DECODE_ABORT);
  }
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
  av_frame_ = av_frame_alloc();
  return true;
}

}  // namespace media