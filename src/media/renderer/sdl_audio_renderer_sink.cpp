#include <iostream>
#include "media/renderer/sdl_audio_renderer_sink.h"

namespace media {
const int kSDLAudioSampleBufferSize = 256;
SdlAudioRendererSink::SdlAudioRendererSink() {
}
SdlAudioRendererSink::~SdlAudioRendererSink() {
}
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
  SDL_PauseAudio(1);
}
void SdlAudioRendererSink::Stop() {
  SDL_PauseAudio(1);
  SDL_CloseAudio();
}

void SdlAudioRendererSink::InitializeSDLAudio() {
  sdl_audio_spec_.freq = audio_parameters_.sample_rate_;
  sdl_audio_spec_.format = AUDIO_S16SYS;  // audio_parameters_.sample_format_;
  sdl_audio_spec_.channels = static_cast<uint8_t>(audio_parameters_.channel_count_);
  sdl_audio_spec_.callback = SdlAudioRendererSink::SdlAudioCallback;
  sdl_audio_spec_.userdata = this;
  sdl_audio_spec_.samples = kSDLAudioSampleBufferSize;
  int sdl_init_result = SDL_Init(SDL_INIT_AUDIO);
  SDL_AudioSpec spec;
  if (SDL_OpenAudio(&sdl_audio_spec_, &spec) < 0) {
    std::cout << "open sdl audio device failed" << std::endl;
    init_cb_(false);
  }
  SDL_PauseAudio(1);
  init_cb_(true);
}

void SdlAudioRendererSink::SdlAudioCallback(void* opaque,
                                            uint8_t* buffer,
                                            int buffer_length) {
  SdlAudioRendererSink* sdl_audio_renderer_sink =
      static_cast<SdlAudioRendererSink*>(opaque);
  sdl_audio_renderer_sink->render_callback_->Render(buffer, buffer_length);
}

}  // namespace media