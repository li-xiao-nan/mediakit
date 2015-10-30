//
//  av_pipeline.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/28.
//
//

#include "av_pipeline.h"

#include "boost/bind.hpp"
#include "base/CommonDef.h"

namespace media {
AVPipeline::AVPipeline(TaskRunner* task_runner)
    : task_runner_(task_runner), state_(STATE_CREATE) {
}

void AVPipeline::Start(std::shared_ptr<Demuxer> demuxer,
                       PipelineStatusCB error_cb,
                       PipelineStatusCB seek_cb) {
  demuxer_ = demuxer;
  error_cb_ = error_cb;
  seek_cb_ = seek_cb;
}

void AVPipeline::StartAction() {
  StateTransitAction(PIPELINE_OK);
}

void AVPipeline::InitializeDemuxer(PipelineStatusCB status_cb) {
  AsyncTask init_task =
      boost::bind(&Demuxer::Initialize, demuxer_.get(), status_cb);
  task_runner_->post(init_task);
}

void AVPipeline::Stop() {
}

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

  PipelineStatusCB task_callback =
      boost::bind(&AVPipeline::StateTransitAction, this, _1);
  SetState(GetNextState());
  switch (state_) {
    case STATE_INIT_DEMUXER:
      InitializeDemuxer(task_callback);
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
    // return STATE_INIT_DECODER;
    case STATE_INIT_DECODER:
    case STATE_SEEKING:
      return STATE_PLAYING;
  }
}
void AVPipeline::SetState(AVPipeline::PipelineState state) {
  state_ = state;
}

}  // namespace media