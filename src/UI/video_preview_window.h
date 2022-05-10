#ifndef UI_VIDEO_PREVIEW_WINDOW_H
#define UI_VIDEO_PREVIEW_WINDOW_H
#include <windows.h>
#include "media/base/video_frame.h"

namespace mediakit {
  LRESULT CALLBACK VideoPreviewProc(::HWND, UINT, WPARAM, LPARAM);
class VideoPreviewWindow {
public:
  VideoPreviewWindow(::HWND hwnd, int x, int y, int width, int height);
  void DrawVideoFrame();
  void OnReceiveVideoFrame(std::shared_ptr<media::VideoFrame> video_frame);
  void ShowWindow(int x, int y, int w, int h);
  void HideWindow();
private:
  void InitBigMapInfo();
  ATOM RegisterWindowClass(HINSTANCE hInstance);
private:
  int x_;
  int y_;
  int w_;
  int h_;
  std::shared_ptr<media::VideoFrame> video_frame_;
  HINSTANCE hInstance_;
  ::HWND hwnd_;
  ::HWND parent_hwnd_;
  BITMAPINFO* bit_map_info_;
};
} // namespace mediakit
#endif