#ifndef MEDIA_RENDERER_AUDIO_RENDERER_SINK_H
#define MEDIA_RENDERER_AUDIO_RENDERER_SINK_H

#include "boost/function.hpp"
#include "media/base/audio_parameters.h"

namespace media {
class AudioRendererSink {
 public:
  typedef boost::function<void(bool)> InitCB;

  class RenderCallback {
   public:
    virtual void Render(uint8_t* data, int data_size) = 0;
    virtual void OnRenderError() = 0;
    virtual ~RenderCallback() = default;
  };
  virtual ~AudioRendererSink() {}
  virtual void Initialize(RenderCallback* render_callback,
                          InitCB init_cb,
                          const AudioParameters& paramters) = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
};
}
#endif