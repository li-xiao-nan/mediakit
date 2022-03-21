//
//  av_pipeline.cpp
//  mediacore
//
//  Created by lixiaonan on 15/8/28.
//
//

#include "av_pipeline.h"
#include "boost/bind.hpp"

#include "log/log_wrapper.h"
#include "base/message_loop_thread_manager.h"

namespace media {
AVPipeline::AVPipeline(std::shared_ptr<Demuxer> demuxer,
  std::shared_ptr<Renderer> renderer,
  PipelineStatusCB error_cb,
  PipelineStatusCB seek_cb,
  VideoRenderer::PaintCB paint_cb)
  : state_(STATE_CREATE)
  , demuxer_(demuxer)
  , renderer_(renderer)
  , error_cb_(error_cb)
  , seek_cb_(seek_cb)
  , paint_cb_(paint_cb){
  demuxer_->SetDelegate(this);
}

void AVPipeline::Start() {
  PostTask(TID_DECODE, boost::bind(&AVPipeline::StartAction, this));
}

void AVPipeline::StartAction() { StateTransitAction(PIPELINE_OK); }

void AVPipeline::InitializeDemuxer(PipelineStatusCB done_cb) {
  demuxer_->Initialize(done_cb);
}

void AVPipeline::InitializeRenderer(PipelineStatusCB done_cb) {
  renderer_->Initialize(demuxer_.get(), done_cb,
                        boost::bind(&AVPipeline::FiltersStatusCB, this, _1),
                        paint_cb_);
}

void AVPipeline::Pause() {
  renderer_->Pause();
  demuxer_->Pause();
}

void AVPipeline::Resume() {
  renderer_->Resume();
  demuxer_->Resume();
}

void AVPipeline::Seek(int64_t timestamp_ms) {
  // pause play
  Pause();
  AsyncTask task =
      boost::bind(&AVPipeline::SeekAction, this, timestamp_ms);
  PostTask(TID_DECODE, task);
}

void AVPipeline::SeekAction(int64_t timestamp_ms) {
  assert(demuxer_.get());
  pending_seek = true;
  state_ = STATE_SEEKING;
  renderer_->Seek(timestamp_ms);

  PipelineStatusCB seek_complete_cb =
      boost::bind(&AVPipeline::StateTransitAction, this, _1);
  LOGGING(LOG_LEVEL_DEBUG)<<"Post Seek Task";
  demuxer_->Seek(timestamp_ms, seek_complete_cb);
}

int64_t AVPipeline::GetPlaybackTime() {
    return renderer_->GetPlaybackTime();
}

void AVPipeline::Stop() {
  renderer_->Stop();
  MessageLoopManager::GetInstance()->StopAll();
}

void AVPipeline::OnUpdateAlignedSeekTimestamp(int64_t seek_timestamp) {
  renderer_->UpdateAlignSeekTimestamp(seek_timestamp);
  LOGGING(LOG_LEVEL_INFO) << "OnUpdateAlignedSeekTimestamp :"
     << seek_timestamp << "; CurrentTimestamp:" << GetPlaybackTime();
  ShowStateInfo();
}

void AVPipeline::StateTransitAction(PipelineStatus status) {
  if (status != PIPELINE_OK) {
    error_cb_(status);
    return;
  }

  PipelineStatusCB done_cb =
      boost::bind(&AVPipeline::StateTransitAction, this, _1);
  SetState(GetNextState());
  switch (state_) {
    case STATE_INIT_DEMUXER:
      InitializeDemuxer(done_cb);
      break;
    case STATE_INIT_RENDERER:
      InitializeRenderer(done_cb);
      break;
    case STATE_PLAYING:
      renderer_->StartPlayingFrom(demuxer_->GetStartTime());
      break;
    case STATE_SEEK_COMPLETED:
      Resume();
      state_ = STATE_PLAYING;
      // do nothing
      break;
  }
}

AVPipeline::PipelineState AVPipeline::GetNextState() {
  switch (state_) {
    case STATE_CREATE:
      return STATE_INIT_DEMUXER;
    case STATE_INIT_DEMUXER:
      LOGGING(LOG_LEVEL_INFO)<< "Demuxer initialize success";
      return STATE_INIT_RENDERER;
    case STATE_INIT_RENDERER:
      LOGGING(LOG_LEVEL_INFO) << "Render initialize success";
      return STATE_PLAYING;
    case STATE_SEEKING:
      return STATE_SEEK_COMPLETED;
    default:
      assert(NULL);
  }
  return STATE_ERROR;
}
void AVPipeline::SetState(AVPipeline::PipelineState state) { state_ = state; }

void AVPipeline::FiltersStatusCB(PipelineStatus filters_status) {
  // todo(lixiaonan): add process filter status logic
}

void AVPipeline::ShowStateInfo() {
  renderer_->ShowStateInfo();
}

}  // namespace media