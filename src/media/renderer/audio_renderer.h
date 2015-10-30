#ifndef MEDIA_RENDERER_AUDIO_RENDERER_H
#define MEDIA_RENDERER_AUDIO_RENDERER_H

namespace media{
class AudioRenderer{
public:
	virtual ~AudioRenderer(){}
	virtual Initialize(DemuxerStream* demuxer_stream,
					   PipelineStatusCB init_cb,
					   PipelineStatusCB status_cb);
	virtual void StartPlayingFrom(int64_t offset) = 0;
	virtual void SetPlaybackRate(float rate) = 0;
	virtual void SetVolume(float volume) = 0;
};
} // namespace media
#endif
