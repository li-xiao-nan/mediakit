#if 1
#ifndef WINDOW_VIDEO_DISPLAY_WINDOW_H
#define WINDOW_VIDEO_DISPLAY_WINDOW_H

#include <windows.h>

#include <memory>

#include "window/yuv_render.h"
#include "base/gl_context_win.h"
#include "media/base/video_frame.h"
namespace mediakit {
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class VideoDisplayWindow {
public:
  VideoDisplayWindow(HWND parent_hwnd, int x, int y, int w, int h);
  void display(std::shared_ptr<media::VideoFrame> video_frame);
private:
  ATOM RegisterWindowClass(HINSTANCE hInstance);
  void Initialize();
  bool is_initialized_;
  HINSTANCE hInstance_;
  HWND hwnd_;
  HWND parent_hwnd_;
  YuvRender render_;
  std::unique_ptr<media::GLContextWin> gl_context_win_;
};
}  // namespace mediakit

#endif
#endif