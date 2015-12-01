#include "audio_renderer_impl.h"

namespace media {
AudioRendererImpl::AudioRendererImpl() {}
void AudioRendererImpl::Initialize(DemuxerStream* demuxer_stream,
                                   PipelineStatusCB init_cb,
                                   PipelineStatusCB status_cb) {}
void AudioRendererImpl::StartPlayingFrom(int64_t offset) {}
void AudioRendererImpl::SetPlaybackRate(float rate) {}
void AudioRendererImpl::SetVolume(float volume) {}
}  // namespace media