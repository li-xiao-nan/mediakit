#include "video_renderer_impl.h"
#include "media/decoder/video_frame_stream.h"

namespace media {
const int kMaxPendingPaintFrameCount = 4;

VideoRendererImpl::VideoRendererImpl(
    TaskRunner* task_runner,
    const VideoFrameStream::VecVideoDecoders& vec_video_decoders)
    : pending_paint_(false),
      state_(STATE_UNINITIALIZED),
      task_runner_(task_runner),
      video_frame_stream_(
          new VideoFrameStream(task_runner, vec_video_decoders)) {}

void VideoRendererImpl::Initialize(DemuxerStream* demuxer_stream,
                                   PipelineStatusCB init_cb,
                                   PipelineStatusCB status_cb, PaintCB paint_cb,
                                   GetTimeCB get_time_cb) {
  init_cb_ = init_cb;
  status_cb_ = status_cb;
  demuxer_stream_ = demuxer_stream;
  paint_cb_ = paint_cb;
  get_time_cb_ = get_time_cb;

  AsyncTask initialize_task =
      boost::bind(&VideoRendererImpl::InitializeAction, this);
  task_runner_->post(initialize_task);
}

void VideoRendererImpl::InitializeAction() {
  state_ = STATE_INITIALIZING;
  video_frame_stream_->Initialize(
      demuxer_stream_,
      boost::bind(&VideoRendererImpl::OnInitializeVideoFrameStreamDone, this,
                  _1));
}

void VideoRendererImpl::OnInitializeVideoFrameStreamDone(bool result) {
  if (!result) {
    init_cb_(VIDEO_RENDERER_INIT_FAILED);
    return;
  }

  state_ = STATE_INITIALIZED;
  init_cb_(PIPELINE_OK);
}

void VideoRendererImpl::StartPlayingFrom(int64_t offset) {
  start_playing_time_ = offset;
  video_frame_paint_thread_.reset(
      new boost::thread(boost::bind(&VideoRendererImpl::ThreadMain, this)));
}

void VideoRendererImpl::SetPlaybackRate(float rate) {}

void VideoRendererImpl::ThreadMain() {
  for (;;) {
    boost::mutex::scoped_lock lock(ready_frames_lock_);
    if (pending_paint_frames_.empty()) {
      ReadFrameIfNeeded();
      frame_available_.wait(lock);
    }

    std::shared_ptr<VideoFrame> next_frame = pending_paint_frames_.front();
    int64_t next_frame_timestamp = next_frame->timestamp_;
    int64_t current_time = get_time_cb_();

    FrameOperation operation =
        DetermineNextFrameOperation(current_time, next_frame_timestamp);
    switch (operation) {
      case OPERATION_DROP_FRAME:
        pending_paint_frames_.pop();
        droped_frame_count_++;
        std::cout << "drop frame: " << droped_frame_count_ << std::endl;
        continue;
        break;
      case OPERATION_PAINT_IMMEDIATELY:
        paint_cb_(next_frame);
        pending_paint_frames_.pop();
        break;
      case OPERATION_WAIT_FOR_PAINT:
        break;
      default:
        break;
    }  // switch
  }    // for(;;)
}

VideoRendererImpl::FrameOperation
VideoRendererImpl::DetermineNextFrameOperation(int64_t current_time,
                                               int64_t next_frame_pts) {
  if (next_frame_pts > current_time) return OPERATION_WAIT_FOR_PAINT;
  int64_t time_delta = current_time - next_frame_pts;
  if (time_delta <= 100) {
    return OPERATION_PAINT_IMMEDIATELY;
  } else {
    return OPERATION_DROP_FRAME;
  }
}

void VideoRendererImpl::OnReadFrameDone(
    VideoFrameStream::Status status, std::shared_ptr<VideoFrame> video_frame) {
  boost::mutex::scoped_lock lock(ready_frames_lock_);

  if (video_frame.get()) {
    pending_paint_frames_.push(video_frame);
    frame_available_.notify_all();
  }

  ReadFrameIfNeeded();
}

void VideoRendererImpl::PaintReadyVideoFrame(
    std::shared_ptr<VideoFrame> video_frame) {
  if (!video_frame.get()) return;
  int64_t current_time = get_time_cb_();
  int64_t video_frame_pts = video_frame->timestamp_;
  if (video_frame_pts < current_time) {
    pending_paint_ = true;
    return;
  }
  if (video_frame_pts - current_time <= 100) {
    paint_cb_(video_frame);
  }
  video_frame.reset();
  pending_paint_ = false;
}

void VideoRendererImpl::ReadFrameIfNeeded() {
  if (pending_paint_frames_.size() >= kMaxPendingPaintFrameCount) return;
  task_runner_->post(boost::bind(&VideoRendererImpl::ReadFrame, this));
}

void VideoRendererImpl::ReadFrame() {
  video_frame_stream_->Read(
      boost::bind(&VideoRendererImpl::OnReadFrameDone, this, _1, _2));
}

}  // namespace media