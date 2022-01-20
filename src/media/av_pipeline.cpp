//
//  av_pipeline.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/28.
//
//

#include "av_pipeline.h"
#include "log/log_wrapper.h"

#include "boost/bind.hpp"

namespace media {
AVPipeline::AVPipeline(TaskRunner* task_runner)
    : task_runner_(task_runner), state_(STATE_CREATE) {}

void AVPipeline::Start(std::shared_ptr<Demuxer> demuxer,
                       std::shared_ptr<Renderer> renderer,
                       PipelineStatusCB error_cb, PipelineStatusCB seek_cb,
                       VideoRenderer::PaintCB paint_cb) {
  demuxer_ = demuxer;
  renderer_ = renderer;
  error_cb_ = error_cb;
  seek_cb_ = seek_cb;
  paint_cb_ = paint_cb;
  task_runner_->post(boost::bind(&AVPipeline::StartAction, this));
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

void AVPipeline::Stop() {}

void AVPipeline::Seek(int64_t timestamp, PipelineStatusCB seek_cb) {
  AsyncTask task =
      boost::bind(&AVPipeline::SeekAction, this, timestamp, seek_cb);
  assert(task_runner_ != NULL);
  task_runner_->post(task);
}

void AVPipeline::SeekAction(int64_t timestamp, PipelineStatusCB seek_cb) {
  assert(seek_cb);
  assert(demuxer_.get());

  seek_cb_ = seek_cb;
  pending_seek = true;
  state_ = STATE_SEEKING;
  PipelineStatusCB seek_complete_cb =
      boost::bind(&AVPipeline::StateTransitAction, this, _1);
  AsyncTask task =
      boost::bind(&Demuxer::Seek, demuxer_.get(), timestamp, seek_complete_cb);
  task_runner_->post(task);
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
    case STATE_SEEKING:
      // do nothing
      break;
  }
}

AVPipeline::PipelineState AVPipeline::GetNextState() {
  switch (state_) {
    case STATE_CREATE:
      return STATE_INIT_DEMUXER;
    case STATE_INIT_DEMUXER:
      LogMessage(LOG_LEVEL_INFO, "Demuxer initialize success");
      return STATE_INIT_RENDERER;
    case STATE_INIT_RENDERER:
      LogMessage(LOG_LEVEL_INFO, "Render initialize success");
      return STATE_PLAYING;
    case STATE_SEEKING:
      return STATE_PLAYING;
    default:
      assert(NULL);
  }
  return STATE_ERROR;
}
void AVPipeline::SetState(AVPipeline::PipelineState state) { state_ = state; }

void AVPipeline::FiltersStatusCB(PipelineStatus filters_status) {
  // todo(lixiaonan): add process filter status logic
}

}  // namespace media