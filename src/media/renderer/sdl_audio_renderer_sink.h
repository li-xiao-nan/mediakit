#ifndef MEDIA_RENDERER_SDL_AUDIO_RENDERER_SINK_H
#define MEDIA_RENDERER_SDL_AUDIO_RENDERER_SINK_H

#include "SDL_audio.h"
#include "SDL.h"

#include "base/macros.h"
#include "media/renderer/audio_renderer_sink.h"
#include "media/base/audio_parameters.h"

namespace media {
class SdlAudioRendererSink : public AudioRendererSink {
 public:
  SdlAudioRendererSink();
  virtual void Initialize(RenderCallback* render_callback,
                          InitCB init_cb,
                          const AudioParameters& parameters) override;
  virtual void Start() override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Stop() override;

 private:
  static void SDLAudioCallback(void* opaque,
                               uint8_t* buffer,
                               int buffer_length);
  void InitializeSDLAudio();

  bool is_playing;
  bool pause_state_;
  InitCB init_cb_;
  SDL_AudioSpec sdl_audio_spec_;
  AudioParameters audio_parameters_;
  RenderCallback* render_callback_;

  DISALLOW_COPY_AND_ASSIGN(SdlAudioRendererSink);
};
}
#endif
