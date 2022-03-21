#if 1

#include <stdio.h>
#include "boost/asio/io_service.hpp"
#include "boost/function.hpp"
#include "media/av_pipeline.h"
#include "net/url.h"
#include "net/io_channel.h"
#include "log/log_wrapper.h"
#include "log/watch_movie_state_recoder.h"
#include "base/message_loop_thread_manager.h"
#include "av_pipeline_factory.h"
#include "window/video_display_window.h"
#include "window/main_message_loop_win.h"

std::unique_ptr<media::AVPipeline> av_pipeline = nullptr;
mediakit::VideoDisplayWindow *video_display_window = nullptr;

void GlobalPaintCallBack(std::shared_ptr<media::VideoFrame> video_frame) {
  if (!video_frame.get()) {
    return;
  }
  video_display_window->display(video_frame);
}

#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;                      // 当前实例
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  video_display_window = new mediakit::VideoDisplayWindow(hInstance);
  std::string video_url = media::UTF16toANSI(lpCmdLine);
  av_pipeline = MakeAVPipeLine(video_url.c_str(), boost::bind(GlobalPaintCallBack, _1));
  av_pipeline->Start();
  int result = mediakit::EnterMainMessageLoop(hInstance);
  av_pipeline->Stop();
  return result;
}

#endif