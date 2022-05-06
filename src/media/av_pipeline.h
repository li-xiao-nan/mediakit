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
#include <list>

#include "boost/function.hpp"
#include "boost/enable_shared_from_this.hpp"

#include "base/base_type.h"
#include "media/demuxer/demuxer.h"
#include "media/renderer/renderer.h"
#include "media/renderer/video_renderer.h"
#include "media/demuxer/demuxer_delegate.h"
#include "media/av_pipeline_observer.h"

namespace media {

class AVPipeline : public boost::enable_shared_from_this<AVPipeline>, 
  public DemuxerDelegate,
  public RendererDelegate {
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
  AVPipeline(const AVPipeline& av_pipeline) = delete;
  AVPipeline(AVPipeline&& av_pipeline) = delete;
  AVPipeline& operator = (const AVPipeline& av_pipeline) = delete;
  void Start();
  void Pause();
  void Resume();
  void Seek(int64_t timestamp_ms);
  void Stop();
  int64_t GetPlaybackTime();
  void AddObserver(AVPipelineObserver* observer);
  void RemoveObserver(AVPipelineObserver* observer);
  
  // DemuxerDelegate impl
  void OnUpdateAlignedSeekTimestamp(int64_t seek_timestamp) override;
  void OnGetMediaInfo(const MediaInfo& media_info) override;
  void OnOpenMediaFileFailed(const std::string file_name, int error_code,
    const std::string& error_description) override;

  // RendererDelegate impl
  void OnPlayProgressUpdate(int timestamp) override;
 private:
  static int GenerateId();
  void StartAction();
  void InitializeDemuxer(PipelineStatusCB done_cb);
  void InitializeRenderer(PipelineStatusCB done_cb);
  void FiltersStatusCB(PipelineStatus filters_status);
  void SeekAction(int64_t timestamp);
  void StateTransitAction(PipelineStatus status);
  PipelineState GetNextState();
  void SetState(PipelineState state);
  void ShowStateInfo();

  bool pending_seek;
  PipelineState state_;
  VideoRenderer::PaintCB paint_cb_;
  std::shared_ptr<Demuxer> demuxer_;
  std::shared_ptr<Renderer> renderer_;
  PipelineStatusCB error_cb_;
  PipelineStatusCB seek_cb_;
  std::list<AVPipelineObserver*> avpipeline_observer_list_;
  int id_;
};

}  // namespace media

#endif
