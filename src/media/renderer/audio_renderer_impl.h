#ifndef MEDIA_RENDERER_IMPL_H
#define MEDIA_RENDERER_IMPL_H

#include "audio_renderer.h"
#include "media/demuxer/demuxer_stream.h"

namespace media {
class AudioRendererImpl : public AudioRenderer {
 public:
  AudioRendererImpl();
  virtual void Initialize(DemuxerStream* demuxer_stream,
                          PipelineStatusCB init_cb,
                          PipelineStatusCB status_cb) override;
  virtual void StartPlayingFrom(int64_t offset) override;
  virtual void SetPlaybackRate(float rate) override;
  virtual void SetVolume(float volume) override;
};
}
#endif