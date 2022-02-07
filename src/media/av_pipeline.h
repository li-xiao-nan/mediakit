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

#include "base/base_type.h"
#include "media/demuxer/demuxer.h"
#include "media/renderer/renderer.h"
#include "media/renderer/video_renderer.h"
#include "media/demuxer/demuxer_delegate.h"

namespace media {

class AVPipeline : public boost::enable_shared_from_this<AVPipeline>, 
  public DemuxerDelegate{
 public:
  enum PipelineState {
    STATE_CREATE,
    STATE_INIT_DEMUXER,
    STATE_INIT_RENDERER,
    STATE_SEEKING,
    STATE_SEEK_COMPLETED,
    STATE_PLAYING,
    STATE_STOPING,
    STATE_STOPPED,
    STATE_ERROR
  };

  AVPipeline(std::shared_ptr<Demuxer> demuxer,
             std::shared_ptr<Renderer> renderer,
             PipelineStatusCB error_cb,
             PipelineStatusCB seek_cb,
             VideoRenderer::PaintCB paint_cb);
  void Start();
  void Stop();
  void Pause();
  void Resume();
  void Seek(int64_t timestamp_ts);
  int64_t GetPlaybackTime();
  
  void OnUpdateAlignedSeekTimestamp(int64_t seek_timestamp) override;
 private:
  void StartAction();
  void InitializeDemuxer(PipelineStatusCB done_cb);
  void InitializeRenderer(PipelineStatusCB done_cb);
  void FiltersStatusCB(PipelineStatus filters_status);
  void SeekAction(int64_t timestamp);
  void StateTransitAction(PipelineStatus status);
  PipelineState GetNextState();
  void SetState(PipelineState state);

  bool pending_seek;
  PipelineState state_;
  VideoRenderer::PaintCB paint_cb_;
  std::shared_ptr<Demuxer> demuxer_;
  std::shared_ptr<Renderer> renderer_;
  PipelineStatusCB error_cb_;
  PipelineStatusCB seek_cb_;
};

}  // namespace media

#endif
