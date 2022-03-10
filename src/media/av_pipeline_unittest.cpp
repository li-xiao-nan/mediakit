#if 0
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
#include "media/renderer/yuv_render.h"
#include "media/renderer/rgb_render.h"



std::unique_ptr<media::AVPipeline> av_pipeline = nullptr;
std::queue<std::shared_ptr<media::VideoFrame> > queue_video_frame;
boost::mutex queue_mutex;
media::WatchMovieStateRecoder file_view_record;
std::string movie_name;
media::YuvRender yuv_render;
media::RgbRender rgb_render;
bool use_yuv_render = true;

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

void init();
void display();
void reshape(int w, int h);
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
  if(use_yuv_render) {
    yuv_render.Init();
  } else {
    rgb_render.Init();
  }
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(display);
  glutMainLoop();
  LOGGING(media::LOG_LEVEL_INFO) << "Playback Time:" << std::to_string(av_pipeline->GetPlaybackTime());
  return 0;
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
  if (use_yuv_render) {
    yuv_render.updateTexture(videoFrame);
    yuv_render.render(winW, winH, videoFrame->_w, videoFrame->_h);
  } else {
    rgb_render.updateTexture(videoFrame);
    rgb_render.render(winW, winH, videoFrame->_w, videoFrame->_h);
  }
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
#endif