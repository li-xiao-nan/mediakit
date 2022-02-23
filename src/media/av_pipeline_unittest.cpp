#include "boost/smart_ptr/scoped_ptr.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/function.hpp"
#include "boost/thread.hpp"
#include <stdio.h>

extern "C" {
#include "GL/glew.h"
}
#include "GL/glut.h"

#include "media/av_pipeline.h"
#include "media/demuxer/ffmpeg_demuxer.h"
#include "media/renderer/renderer.h"
#include "media/renderer/audio_renderer.h"
#include "media/renderer/video_renderer.h"
#include "media/decoder/audio_decoder.h"
#include "media/decoder/video_decoder.h"
#include "media/renderer/video_renderer_impl.h"
#include "media/renderer/audio_renderer_impl.h"
#include "media/decoder/ffmpeg_video_decoder.h"
#include "media/decoder/ffmpeg_audio_decoder.h"
#include "net/url.h"
#include "net/io_channel.h"
#include "log/log_wrapper.h"
#include "log/watch_movie_state_recoder.h"
#include "base/message_loop_thread_manager.h"
#include "av_pipeline_factory.h"

std::unique_ptr<media::AVPipeline> av_pipeline = nullptr;
std::queue<std::shared_ptr<media::VideoFrame> > queue_video_frame;
boost::mutex queue_mutex;
media::WatchMovieStateRecoder file_view_record;
std::string movie_name;

void GlobalPaintCallBack(std::shared_ptr<media::VideoFrame> video_frame) {
  boost::mutex::scoped_lock lock(queue_mutex);
  queue_video_frame.push(video_frame);
}


std::shared_ptr<media::VideoFrame> GlobalReadVideoFrame() {
  boost::mutex::scoped_lock lock(queue_mutex);
  std::shared_ptr<media::VideoFrame> video_frame;
  if (queue_video_frame.empty())
    return video_frame;
  video_frame = queue_video_frame.front();
  queue_video_frame.pop();
  return video_frame;
}

#define USE_YUV
void init();
void setShader();
void display();
void reshape(int w, int h);
void updateTexture(std::shared_ptr<media::VideoFrame> image);
void xcb() {
  glutPostRedisplay();
}

void ycb() {
  glutPostRedisplay();
}
void zcb() {
  glutPostRedisplay();
}

void ExitApp(){
  LOGGING(media::LOG_LEVEL_INFO) << "Playback Time:" << av_pipeline->GetPlaybackTime();
  file_view_record.RecordFilmViewProgress(movie_name, av_pipeline->GetPlaybackTime());
  file_view_record.WriteToLocalFile();
  exit(0);
}

bool pause_state = false;
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 's':
      av_pipeline->Seek(60*20*1000 + av_pipeline->GetPlaybackTime());
      break;
    case 'p':
      if(pause_state == false) {
        av_pipeline->Pause();
        pause_state = true;
      } else {
        av_pipeline->Resume();
        pause_state = false;
      }
      break;
    case 'z':
      zcb();
      break;
    case 'q':
      ExitApp();
      break;
    default:
      break;
  }
}

int winW = 80, winH = 60;
int videoW, videoH;

GLuint vsID, fsID, pID;
bool hasInitTex;
int main(int argc, char* argv[]) {
  file_view_record.Initialize();
  int64_t x1_timestamp = file_view_record.GetFilmViewProgress("x1");
  file_view_record.RecordFilmViewProgress("x2", 10304104);
  file_view_record.WriteToLocalFile();

  if (argc < 2){
      std::cout << "you need input video file!" << std::endl;
      exit(0);
  }
  movie_name = media::GetMovieNameUtf8(argv[1]);
  av_pipeline = MakeAVPipeLine(argv[1], boost::bind(GlobalPaintCallBack, _1));
  av_pipeline->Start();

  // glut initialize
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(winW, winH);
  glutCreateWindow(argv[1]);
  init();
  glewInit();
  setShader();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(display);
  glutMainLoop();
  LOGGING(media::LOG_LEVEL_INFO) << "Playback Time:" << std::to_string(av_pipeline->GetPlaybackTime());
  return 0;
}
#ifdef USE_YUV
GLuint texname[3];
#else
GLuint texname;
#endif
void initTexture(std::shared_ptr<media::VideoFrame> image) {
#ifdef USE_YUV
  glGenTextures(3, texname);
#else
  glGenTextures(1, &texname);
#endif
#ifdef USE_YUV
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
#elif USE_YUV_RGB

  glBindTexture(GL_TEXTURE_2D, texname);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->_w, image->_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image->_yuv);
#else
  glBindTexture(GL_TEXTURE_2D, texname);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->_w, image->_h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image->_data);

#endif
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
}
void display(void) {
  if(pause_state) return;
  std::shared_ptr<media::VideoFrame> videoFrame = GlobalReadVideoFrame();
  if (!videoFrame.get()) {
    return;
  }
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  videoH = videoFrame->_h;
  videoW = videoFrame->_w;
  updateTexture(videoFrame);
  float playingH = videoH * (winW * 1.0f) / (videoW * 1.0f);
  float playingYPos = (winH * 1.0f - playingH) / 2.0f;
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(0.0, playingYPos);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(winW*1.0f, playingYPos);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(winW*1.0f, playingYPos + playingH);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(0.0, playingYPos + playingH);
  glEnd();
  glFlush();
}
void reshape(int w, int h) {
  winW = w;
  winH = h;
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void updateTexture(std::shared_ptr<media::VideoFrame> image) {
  if (!hasInitTex) {
    initTexture(image);
    hasInitTex = true;
  } else {
#ifdef USE_YUV
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
#elif USE_YUV_RGB
    glBindTexture(GL_TEXTURE, texname);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_w, image->_h, GL_RGB,
                    GL_UNSIGNED_BYTE, image->_yuv);
#else
    glBindTexture(GL_TEXTURE, texname);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->_w, image->_h, GL_RGB,
                    GL_UNSIGNED_BYTE, image->_data);
#endif
  }
}

const char* vsSrc =
    "void main() \
  { gl_Position = ftransform();\
    gl_TexCoord[0]=gl_MultiTexCoord0;}";

const char* fsSrc =
#ifdef USE_YUV
    "uniform sampler2D texY, texU, texV; \
void main() { \
  vec4 c = vec4((texture2D(texY, gl_TexCoord[0].st).r-16./255.)*1.164);\
  vec4 U = vec4(texture2D(texU, gl_TexCoord[0].st).r-128./255.);\
  vec4 V = vec4(texture2D(texV, gl_TexCoord[0].st).r-128./255.);\
  c += V*vec4(1.596, -0.813, 0, 0);\
  c += U*vec4(0, -0.392, 2.017, 0); \
  c.a = 1.0;\
  gl_FragColor = c;}";

/*
"uniform sampler2D texY, texU, texV; \
 void main(){\
  float nx, ny, r, g, b, y, u, v;\
  vec4 txl, ux,vx; \
  nx = gl_TexCoord[0].x; \
  ny = gl_TexCoord[0].y; \
  y = texture2D(texY, vec2(nx, ny)).r;\
  u = texture2D(texU, vec2(nx/2.0, ny/2.0)).r;\
  v = texture2D(texV, vec2(nx/2.0, ny/2.0)).r;\
  y = 1.1643*(y-0.0625);\
  u = u-0.5; \
  v = v-0.5;  \
  \
  r = y+1.5958*v; \
  g = y-0.39173*u-0.81290*v; \
  b = y+2.017*u; \
  gl_FragColor = vec4(r, g, b, 1.0); }";*/
#elif USE_YUV_RGB
    "uniform sampler2D tex; \
  void main() {\
  vec4 yuv = texture2D(tex,gl_TexCoord[0].st); \
        vec4 color;\
        color.r =yuv.r + 1.4022 * yuv.b - 0.7011;\
        color.r = (color.r < 0.0) ? 0.0 : ((color.r > 1.0) ? 1.0 : color.r);\
        color.g =yuv.r - 0.3456 * yuv.g - 0.7145 * yuv.b + 0.53005;\
        color.g = (color.g < 0.0) ? 0.0 : ((color.g > 1.0) ? 1.0 : color.g);\
        color.b =yuv.r + 1.771 * yuv.g - 0.8855;\
        color.b = (color.b < 0.0) ? 0.0 : ((color.b > 1.0) ? 1.0 : color.b);\
  gl_FragColor = color;}";
#else
    "uniform sampler2D tex;\
  void main() {\
  vec4 color; \
  color = texture2D(tex, gl_TexCoord[0].st);\
  gl_FragColor = color;}";
#endif
void examShaderCompileResult(GLuint obj) {
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

void examProgramInfoLog(GLuint obj) {
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
void setShader() {
  vsID = glCreateShader(GL_VERTEX_SHADER);
  fsID = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vsID, 1, &vsSrc, NULL);
  glShaderSource(fsID, 1, &fsSrc, NULL);

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
