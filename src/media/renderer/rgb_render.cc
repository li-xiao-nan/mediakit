#include "media/renderer/rgb_render.h"

extern "C" {
#include "GL/glew.h"
}

namespace media {
void RgbRender::Init() {
  if (is_init_)
    return;
  is_init_ = true;
  vsID = glCreateShader(GL_VERTEX_SHADER);
  fsID = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vsID, 1, &vs_src, NULL);
  glShaderSource(fsID, 1, &fs_src, NULL);

  glCompileShader(vsID);
  glCompileShader(fsID);

  examShaderCompileResult(vsID);
  examShaderCompileResult(fsID);

  pID = glCreateProgram();
  glAttachShader(pID, vsID);
  glAttachShader(pID, fsID);

  glLinkProgram(pID);
  examProgramInfoLog(pID);

  glUseProgram(pID);
}
  void RgbRender::initializeTexture(std::shared_ptr<media::VideoFrame> image) {
    glBindTexture(GL_TEXTURE_2D, texture_name_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->_w, image->_h, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, image->_data);
    is_init_texture_ = true;
  }

  void RgbRender::updateTexture(std::shared_ptr<media::VideoFrame> image) {
    if(!is_init_texture_) {
      initializeTexture(image);
    }
    glBindTexture(GL_TEXTURE, texture_name_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_w, image->_h, GL_RGBA,
                    GL_UNSIGNED_BYTE, image->_data);
  }

  void RgbRender::render(int window_w, int window_h, int video_w, int video_h) {
    float playingH = video_h * (window_w * 1.0f) / (video_w * 1.0f);
    float playingYPos = (window_h * 1.0f - playingH) / 2.0f;
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(0.0, playingYPos);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(window_w * 1.0f, playingYPos);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(window_w * 1.0f, playingYPos + playingH);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(0.0, playingYPos + playingH);
    glEnd();
    glFlush();
  }

  void RgbRender::examShaderCompileResult(GLuint obj) {
    int infoLogLength = 0;
    int charsWritten = 0;
    char* infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
      infoLog = (char*)malloc(infoLogLength);
      glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
      printf("log-->%s\n", infoLog);
      free(infoLog);
    }
  }

  void RgbRender::examProgramInfoLog(GLuint obj) {
    int logLen = 0;
    int charsWritten = 0;
    char* infoLog = NULL;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0) {
      infoLog = (char*)malloc(logLen);
      glGetProgramInfoLog(obj, logLen, &charsWritten, infoLog);
      printf("log-->%s\n", infoLog);
      free(infoLog);
    }
  }
  } // namespace media