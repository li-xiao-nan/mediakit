#ifndef MEDIA_RENDERER_RENDERER_IMPL_H
#define MEDIA_RENDERER_RENDERER_IMPL_H

#include "base/base_type.h"

#include <map>
#include <memory>

#include "media/base/pipeline_status.h"
#include "media/renderer/renderer.h"
#include "media/renderer/audio_renderer.h"
#include "media/renderer/video_renderer.h"
#include "media/decoder/audio_decoder.h"
#include "media/decoder/video_decoder.h"
#include "media/demuxer/demuxer_stream_provider.h"
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

namespace media {
class TimeSource;

class RendererImpl : public Renderer {
 public:
  static std::shared_ptr<TimeSource> GetPlaybackClock(int Id);

  RendererImpl(std::shared_ptr<AudioRenderer> audio_renderer,
               std::shared_ptr<VideoRenderer> video_renderer);
  ~RendererImpl();
  virtual void Initialize(DemuxerStreamProvider* demuxer_stream_provider,
                          PipelineStatusCB init_cb,
                          PipelineStatusCB status_cb,
                          VideoRenderer::PaintCB paint_cb) override;
  virtual void StartPlayingFrom(int64_t time_offset) override;
  virtual void SetPlaybackRate(float rate) override;
  virtual void SetVolume(float volume) override;
  virtual int64_t GetPlaybackTime() override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Seek(int64_t) override;
  virtual void UpdateAlignSeekTimestamp(int64_t timestamp) override;
  virtual void ShowStateInfo() override;

 private:
  enum State {
    STATE_UNINITIALIZED,
    STATE_AUDIO_RENDERER_INITIALIZING,
    STATE_VIDEO_RENDERER_INITIALIZING,
    STATE_AUDIO_RENDERER_INIT_FAILED,
    STATE_VIDEO_RENDERER_INIT_FAILED,
    STATE_NORMAL,
    STATE_ERROR,
  };

  void InitializeAction();

  void InitializeAudioRenderer();
  void OnInitializeAudioRendererDone(PipelineStatus status);

  void InitializeVideoRenderer();
  void OnInitializeVideoRendererDone(PipelineStatus status);

  int64_t GetCurrentTime();

  static std::map<int, std::shared_ptr<TimeSource>> playback_clock_map_;

  VideoRenderer::PaintCB paint_cb_;
  PipelineStatusCB init_cb_;
  PipelineStatusCB status_cb_;
  State state_;
  std::shared_ptr<TimeSource> playback_clock_;
  DemuxerStreamProvider* demuxer_stream_provider_;
  std::shared_ptr<AudioRenderer> audio_renderer_;
  std::shared_ptr<VideoRenderer> video_renderer_;
};
}  // namespace media
#endif
