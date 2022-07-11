#ifndef MEDIA_RENDERER_IMPL_H
#define MEDIA_RENDERER_IMPL_H

#include <queue>
#include "boost/bind.hpp"
#include "boost/thread/thread.hpp"

#include "audio_renderer.h"
#include "base/macros.h"
#include "base/sonic/sonic.h"
#include "media/demuxer/demuxer_stream.h"
#include "media/renderer/audio_renderer_sink.h"
#include "media/base/pipeline_status.h"
#include "media/base/audio_frame.h"
#include "media/decoder/audio_frame_stream.h"

namespace media {
class AudioFrameStream;

class AudioRendererImpl : public AudioRenderer,
                          public AudioRendererSink::RenderCallback {
 public:
  AudioRendererImpl(
      const AudioFrameStream::VecAudioDecoders& vec_audio_decoders);

  void Initialize(DemuxerStream* demuxer_stream,
                          PipelineStatusCB init_cb,
                          PipelineStatusCB status_cb,
                          GetTimeCB get_time_cb) override;
  void StartPlayingFrom(int64_t offset) override;
  void SetPlaybackRate(float rate) override;
  void SetVolume(float volume) override;
  void Pause() override;
  void Resume() override;
  void ClearAVFrameBuffer() override;
  void ShowStateInfo() override;
  void Stop() override;
  void SetDelegate(AudioRendererDelegate* delegate) override;

  //
  virtual void Render(uint8_t* data, int data_size);
  virtual void OnRenderError();

 private:
  enum State {
    STATE_UNINITIALIZED,
    STATE_INIT_DECODER,
    STATE_INIT_SINKER,
    STATE_PLAYING,
    STATE_FLUSHING,
  };

  void InitAudioRendererFlow(bool result);
  State GetNextState();
  void OnReadFrameDone(AudioFrameStream::Status status,
                       std::shared_ptr<AudioFrame> audio_frame);
  void ReadFrameIfNeeded();
  void ReadReadyFrameLocked();
  void InitAudioRenderSink(const AudioDecoderConfig& audio_decoder_config,
                           AudioRendererSink::InitCB init_cb);
  void SpeedUpAudio(std::shared_ptr<AudioFrame> audio_frame);
  void CreateSonicStream(int sample_rate, int channel_count);
  
  bool pending_read_;
  float playback_rate_;
  std::queue<std::shared_ptr<AudioFrame> > ready_audio_frames_;
  std::queue<std::shared_ptr<AudioFrame> > pending_paint_frames_;
  AudioFrameStream* audio_frame_stream_;
  AudioRendererSink* audio_renderer_sink_;
  DemuxerStream* demuxer_stream_;
  sonicStream sonic_stream_;
  State state_;
  boost::mutex frame_queue_mutex_;
  PipelineStatusCB init_cb_;
  PipelineStatusCB status_cb_;
  GetTimeCB get_time_cb_;
  bool is_waiting_;
  int64_t begin_wait_timestamp_;
  AudioRendererDelegate* delegate_;
  bool is_first_frame_;

  DISALLOW_COPY_AND_ASSIGN(AudioRendererImpl);
};
}
#endif