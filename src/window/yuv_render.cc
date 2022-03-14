#include "window/yuv_render.h"

#include "log/log_wrapper.h"

namespace mediakit {
void YuvRender::Init() {
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

void YuvRender::SetViewport(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void YuvRender::examShaderCompileResult(GLuint obj) {
  int infoLogLength = 0;
  int charsWritten = 0;
  char* infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0) {
    infoLog = (char*)malloc(infoLogLength);
    glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
    LOGGING(media::LOG_LEVEL_ERROR) << infoLog;
    free(infoLog);
  }
}

void YuvRender::examProgramInfoLog(GLuint obj) {
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
void YuvRender::initializeTexture(std::shared_ptr<media::VideoFrame> image) {
  if (is_init_texture_)
    return;
  is_init_texture_ = true;

  glGenTextures(3, texname);
  GLint yTex, uTex, vTex;
  yTex = glGetUniformLocation(pID, "texY");
  uTex = glGetUniformLocation(pID, "texU");
  vTex = glGetUniformLocation(pID, "texV");

  glBindTexture(GL_TEXTURE_2D, texname[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image->_yuvStride[0],
               image->_yuvLineCnt[0], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
               image->_yuvData[0]);

  glBindTexture(GL_TEXTURE_2D, texname[1]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image->_yuvStride[1],
               image->_yuvLineCnt[1], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
               image->_yuvData[1]);

  glBindTexture(GL_TEXTURE_2D, texname[2]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image->_yuvStride[2],
               image->_yuvLineCnt[2], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
               image->_yuvData[2]);

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texname[0]);
  glUniform1i(yTex, 0);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texname[1]);
  glUniform1i(uTex, 1);

  glActiveTexture(GL_TEXTURE2);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texname[2]);
  glUniform1i(vTex, 2);
}

void YuvRender::updateTexture(std::shared_ptr<media::VideoFrame> image) {
  if (!is_init_texture_) {
    initializeTexture(image);
  }
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texname[0]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_yuvStride[0],
                  image->_yuvLineCnt[0], GL_LUMINANCE, GL_UNSIGNED_BYTE,
                  image->_yuvData[0]);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texname[1]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_yuvStride[1],
                  image->_yuvLineCnt[1], GL_LUMINANCE, GL_UNSIGNED_BYTE,
                  image->_yuvData[1]);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, texname[2]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_yuvStride[2],
                  image->_yuvLineCnt[2], GL_LUMINANCE, GL_UNSIGNED_BYTE,
                  image->_yuvData[2]);
}

void YuvRender::render(int window_w, int window_h, int video_w, int video_h) {
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
} // namespace media