#include <math.h>

#include <iostream>

#include "media/renderer/sdl_audio_renderer_sink.h"

namespace media {
const int kSDLMinAudioBufferSize = 512;
const int kSDLAudioMaxCallbacksPerSec = 30;

SdlAudioRendererSink::SdlAudioRendererSink() : pause_state_(false) {}
void SdlAudioRendererSink::Initialize(RenderCallback* render_callback,
                                      InitCB init_cb,
                                      const AudioParameters& parameters) {
  render_callback_ = render_callback;
  audio_parameters_ = parameters;
  init_cb_ = init_cb;
  InitializeSDLAudio();
}
void SdlAudioRendererSink::Start() {
  SDL_PauseAudio(0);
}
void SdlAudioRendererSink::Pause() {
  if(pause_state_) { 
    return;
  } else {
    SDL_PauseAudio(1);
    pause_state_ = true;
  }
  return;
}

void SdlAudioRendererSink::Resume() {
  SDL_PauseAudio(0);
  pause_state_ = false;
}
void SdlAudioRendererSink::Stop() {
  SDL_PauseAudio(1);
  SDL_CloseAudio();
}

void SdlAudioRendererSink::InitializeSDLAudio() {
  sdl_audio_spec_.freq = audio_parameters_.sample_rate_;
  sdl_audio_spec_.format = AUDIO_S16SYS;  // audio_parameters_.sample_format_;
  // SDL 只支持单声道和双声道，如果设置声道数>2,则会初始化失败
  // TODO(lixiaonan):调研支持多声道解决方案
  sdl_audio_spec_.channels = std::min<uint8_t>(2, static_cast<uint8_t>(audio_parameters_.channel_count_));
  sdl_audio_spec_.callback = SdlAudioRendererSink::SDLAudioCallback;
  sdl_audio_spec_.userdata = this;
  sdl_audio_spec_.samples = std::max(kSDLMinAudioBufferSize, 2 << static_cast<int>((log2(sdl_audio_spec_.freq / kSDLAudioMaxCallbacksPerSec))));
  int sdl_init_result = SDL_Init(SDL_INIT_AUDIO);
  SDL_AudioSpec spec;
  if (SDL_OpenAudio(&sdl_audio_spec_, &spec) < 0) {
    BOOST_ASSERT_MSG(0, "open sdl audio device failed");
    std::cout << "open sdl audio device failed" << std::endl;
    init_cb_(false);
  }
  SDL_PauseAudio(1);
  init_cb_(true);
}

void SdlAudioRendererSink::SDLAudioCallback(void* opaque,
                                            uint8_t* buffer,
                                            int buffer_length) {
  SdlAudioRendererSink* sdl_audio_renderer_sink =
      static_cast<SdlAudioRendererSink*>(opaque);
  sdl_audio_renderer_sink->render_callback_->Render(buffer, buffer_length);
}

}  // namespace media