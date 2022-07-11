#ifndef MEDIA_RENDERER_AUDIO_RENDERER_H
#define MEDIA_RENDERER_AUDIO_RENDERER_H

#include "media/base/pipeline_status.h"
#include "media/demuxer/demuxer_stream.h"

namespace media {
class AudioRendererDelegate {
 public:
  virtual void OnGetFirstAudioFrame(int64_t pts) = 0;
};

class AudioRenderer {
 public:
  typedef boost::function<int64_t(void)> GetTimeCB;

  virtual ~AudioRenderer() = default;
  virtual void Initialize(DemuxerStream* demuxer_stream,
                          PipelineStatusCB init_cb, PipelineStatusCB status_cb,
                          GetTimeCB get_time_cb) = 0;
  virtual void StartPlayingFrom(int64_t offset) = 0;
  virtual void SetPlaybackRate(float rate) = 0;
  virtual void SetVolume(float volume) = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void ClearAVFrameBuffer() = 0;
  virtual void ShowStateInfo() = 0;
  virtual void Stop() = 0;
  virtual void SetDelegate(AudioRendererDelegate* delegate) = 0;
};
}  // namespace media
#endif
