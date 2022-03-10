#ifndef MEDIA_RENDERER_YUV_RENDER_H
#define MEDIA_RENDERER_YUV_RENDER_H

#include <memory>
#include "media/base/video_frame.h"

extern "C" {
#include "GL/glew.h"
}

namespace media {
class YuvRender {
 public:
  YuvRender() : is_init_(false), is_init_texture_(false) {}
  void Init();
  void SetViewport(int w, int h);
  void updateTexture(std::shared_ptr<media::VideoFrame> image);
  void render(int window_w, int window_h, int video_w, int video_h);

 private:
  void examShaderCompileResult(GLuint obj);
  void examProgramInfoLog(GLuint obj);
  void initializeTexture(std::shared_ptr<media::VideoFrame> image);

 private:
  bool is_init_;
  bool is_init_texture_;
  GLuint vsID, fsID, pID;
  GLuint texname[3];
  const char* vs_src =
      "void main() \
  { gl_Position = ftransform();\
    gl_TexCoord[0]=gl_MultiTexCoord0;}";
  const char* fs_src =
      "uniform sampler2D texY, texU, texV; \
    void main() { \
    vec4 c = vec4((texture2D(texY, gl_TexCoord[0].st).r-16./255.)*1.164);\
    vec4 U = vec4(texture2D(texU, gl_TexCoord[0].st).r-128./255.);\
    vec4 V = vec4(texture2D(texV, gl_TexCoord[0].st).r-128./255.);\
    c += V*vec4(1.596, -0.813, 0, 0);\
    c += U*vec4(0, -0.392, 2.017, 0); \
    c.a = 1.0;\
    gl_FragColor = c;}";
};
} // namespace media
#endif