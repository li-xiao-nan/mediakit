#ifndef MEDIA_RENDERER_RGB_RENDER_H
#define MEDIA_RENDERER_RGB_RENDER_H

#include <memory>
#include "media/base/video_frame.h"

extern "C" {
#include "GL/glew.h"
}

namespace media {
class RgbRender {
 public:
   RgbRender() : is_init_(false), is_init_texture_(false) {}
  void Init();
  void updateTexture(std::shared_ptr<media::VideoFrame> image);
  void render(int window_w, int window_h, int video_w, int video_h);

 private:
  void initializeTexture(std::shared_ptr<media::VideoFrame> image);
  void examShaderCompileResult(GLuint obj);
  void examProgramInfoLog(GLuint obj);

 private:
  GLuint texture_name_;
  bool is_init_;
  bool is_init_texture_;
  GLuint vsID, fsID, pID;
  const char* vs_src =
      "void main() \
  { gl_Position = ftransform();\
    gl_TexCoord[0]=gl_MultiTexCoord0;}";
  const char* fs_src =
      "uniform sampler2D tex;\
  void main() {\
  vec4 color; \
  color = texture2D(tex, gl_TexCoord[0].st);\
  gl_FragColor = color;}";
};
} // namespace media
#endif