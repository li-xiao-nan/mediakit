#include "video_renderer_impl.h"
#include "media/decoder/video_frame_stream.h"
#include "base/message_loop_thread_manager.h"
#include "log/log_wrapper.h"

namespace media {
const int kMaxPendingPaintFrameCount = 2<<2;
const int kMaxTimeDelta = 100;  // ms

VideoRendererImpl::VideoRendererImpl(
    const VideoFrameStream::VecVideoDecoders& vec_video_decoders)
    : pending_paint_(false),
      pause_state_(false),
      state_(STATE_UNINITIALIZED),
      video_frame_stream_(
          new VideoFrameStream(vec_video_decoders)),
          droped_frame_count_(0) {
}

void VideoRendererImpl::Initialize(DemuxerStream* demuxer_stream,
                                   PipelineStatusCB init_cb,
                                   PipelineStatusCB status_cb,
                                   PaintCB paint_cb,
                                   GetTimeCB get_time_cb) {
  init_cb_ = init_cb;
  status_cb_ = status_cb;
  demuxer_stream_ = demuxer_stream;
  paint_cb_ = paint_cb;
  get_time_cb_ = get_time_cb;

  AsyncTask initialize_task =
      boost::bind(&VideoRendererImpl::InitializeAction, this);
  PostTask(TID_DECODE,initialize_task);
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

void VideoRendererImpl::SetPlaybackRate(float rate) {
}

std::wstring toWString(int64_t value){
  std::wostringstream wstream;
  wstream << value;
  return wstream.str();
}

void LogDecodeInfo(std::shared_ptr<VideoFrame> frame){

  std::wstring log_item = L"FNO." + toWString(frame->_timeRecoder._frameNo);
  log_item += L" dst:" + toWString(frame->_timeRecoder._decodeExpendTime);
  log_item += L" Add:" + toWString(frame->_timeRecoder._popupTime);
  log_item += L" Pst:" + toWString(frame->_timeRecoder._pst);
  log_item += L" Pop:" + toWString(frame->_timeRecoder._popupTime);
  log_item += L" Render:" + frame->_timeRecoder._renderResult;
}

void VideoRendererImpl::Pause() {
  if(pause_state_) return;
  pause_state_ = true;
}

void VideoRendererImpl::Resume() {
  EndPauseState();
}

void VideoRendererImpl::EnterPauseStateIfNeeded() {
  if(pause_state_ == false){
    return;
  }
  LogMessage(LOG_LEVEL_INFO, "VideoRenderer enter into pause state");
  std::unique_lock<std::mutex> lock(mutex_for_pause_);
  condition_variable_for_puase_.wait(lock);
}

void VideoRendererImpl::EndPauseState() {
  if(pause_state_ == true){
    std::unique_lock<std::mutex> lock(mutex_for_pause_);
    condition_variable_for_puase_.notify_all();
    pause_state_ = false;
  }else {
    // do nothing
  }
  return;
}
void VideoRendererImpl::ThreadMain() {
  for (;;) {
    EnterPauseStateIfNeeded();
    boost::mutex::scoped_lock lock(ready_frames_lock_);
    int64_t beg_time = get_time_cb_();
    if (pending_paint_frames_.empty()) {
      ReadFrameIfNeeded();
      int64_t begin_wait_timestamp = get_time_cb_();
      LogMessage(LOG_LEVEL_DEBUG, "video decoded frame is empty, begin wait");
      ScopeTimeCount ScopeTimeCount("Wait decode time:");
      frame_available_.wait(lock);
    }

    std::shared_ptr<VideoFrame> next_frame = pending_paint_frames_.front();
    int64_t next_frame_timestamp = next_frame->timestamp_;
    static int64_t pre_timestamp;
    int64_t current_time = get_time_cb_();

    FrameOperation operation =
        DetermineNextFrameOperation(current_time, next_frame_timestamp);
    switch (operation) {
      case OPERATION_DROP_FRAME:
        next_frame->_timeRecoder._popupTime = get_time_cb_();
        next_frame->_timeRecoder._renderResult = L"drop";
        LogDecodeInfo(next_frame);
        pending_paint_frames_.pop();
        continue;
        break;
      case OPERATION_PAINT_IMMEDIATELY:
        next_frame->_timeRecoder._popupTime = get_time_cb_();
        next_frame->_timeRecoder._renderResult = L"display";
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
  if (next_frame_pts > current_time)
    return OPERATION_WAIT_FOR_PAINT;

  int64_t time_delta = current_time - next_frame_pts;

  if (time_delta <= kMaxTimeDelta) {
    return OPERATION_PAINT_IMMEDIATELY;
  } else {
    return OPERATION_DROP_FRAME;
  }
}

void VideoRendererImpl::OnReadFrameDone(
    VideoFrameStream::Status status,
    std::shared_ptr<VideoFrame> video_frame) {
  boost::mutex::scoped_lock lock(ready_frames_lock_);

  if (video_frame.get()) {
    pending_paint_frames_.push(video_frame);
    frame_available_.notify_all();
  }

  ReadFrameIfNeeded();
}


void VideoRendererImpl::ReadFrameIfNeeded() {
  if (pending_paint_frames_.size() >= kMaxPendingPaintFrameCount)
    return;
  PostTask(TID_DECODE, boost::bind(&VideoRendererImpl::ReadFrame, this));
}

void VideoRendererImpl::ReadFrame() {
  video_frame_stream_->Read(
      boost::bind(&VideoRendererImpl::OnReadFrameDone, this, _1, _2));
}

}  // namespace media
