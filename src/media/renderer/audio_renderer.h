#ifndef MEDIA_RENDERER_AUDIO_RENDERER_H
#define MEDIA_RENDERER_AUDIO_RENDERER_H

#include "media/demuxer/demuxer_stream.h"
#include "media/base/pipeline_status.h"

namespace media {
class AudioRenderer {
 public:
  virtual ~AudioRenderer() {}
  virtual void Initialize(DemuxerStream* demuxer_stream,
                          PipelineStatusCB init_cb,
                          PipelineStatusCB status_cb) = 0;
  virtual void StartPlayingFrom(int64_t offset) = 0;
  virtual void SetPlaybackRate(float rate) = 0;
  virtual void SetVolume(float volume) = 0;
};
}  // namespace media
#endif
