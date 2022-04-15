#include <stdio.h>
#include "boost/asio/io_service.hpp"
#include "boost/function.hpp"
#include "media/av_pipeline.h"
#include "net/url.h"
#include "net/io_channel.h"
#include "log/log_wrapper.h"
#include "log/watch_movie_state_recoder.h"
#include "base/message_loop_thread_manager.h"
#include "window/main_message_loop_win.h"
#include "player/media_player.h"
#include "ui/main_window.h"
// 全局变量:
HINSTANCE hInst;                      // 当前实例
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  std::shared_ptr<mediakit::MainWindow> main_window(new mediakit::MainWindow());

  RECT rects;
  ::GetClientRect(main_window->GetWindowHandle(), &rects);
  int w = rects.right - rects.left;
  int h = rects.bottom - rects.top;
  std::string video_url = media::UTF16toANSI(lpCmdLine);
  std::shared_ptr<mediakit::MediaPlayer> mediaplayer = 
    mediakit::MediaPlayer::CreateMediaPlayer(main_window->GetWindowHandle(), 0, 0, w, h-30, video_url);
  if(mediaplayer == nullptr) return -1;
  mediaplayer->SetClient(main_window);
  mediaplayer->Start();
  int result = mediakit::EnterMainMessageLoop(hInstance);
  mediaplayer->Stop();
  return result;
}