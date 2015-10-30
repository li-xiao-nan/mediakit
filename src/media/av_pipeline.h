//
//  av_pipeline.h
//  mediacore
//
//  Created by 李楠 on 15/8/28.
//
//

#ifndef MEDIA_AV_PIPELINE_H_
#define MEDIA_AV_PIPELINE_H_

#include <memory>

#include "boost/function.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "base/CommonDef.h"
#include "media/demuxer/demuxer.h"

namespace media {

class AVPipeline : public boost::enable_shared_from_this<AVPipeline> {
 public:
  enum PipelineState {
    STATE_CREATE,
    STATE_INIT_DEMUXER,
    STATE_INIT_DECODER,
    STATE_SEEKING,
    STATE_PLAYING,
    STATE_STOPING,
    STATE_STOPPED
  };

  AVPipeline(TaskRunner* task_runner);
  void Start(std::shared_ptr<Demuxer> demuxer,
             PipelineStatusCB error_cb,
             PipelineStatusCB seek_cb);
  void Stop();
  void Seek(int64_t timestamp, PipelineStatusCB seek_cb);

 private:
  void StartAction();
  void InitializeDemuxer(PipelineStatusCB status_cb);
  void SeekAction(int64_t timestamp, PipelineStatusCB seek_cb);
  void StateTransitAction(PipelineStatus status);
  PipelineState GetNextState();
  void SetState(PipelineState state);

  bool pending_seek;
  PipelineState state_;
  TaskRunner* task_runner_;
  std::shared_ptr<Demuxer> demuxer_;
  PipelineStatusCB error_cb_;
  PipelineStatusCB seek_cb_;
};

}  // namespace media

#endif
