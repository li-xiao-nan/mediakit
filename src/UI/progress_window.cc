#include "ui/progress_window.h"

#include <CommCtrl.h >

namespace mediakit {
static const int kPBHeight = 6;
HWND ProgressWindow::hwnd_ = 0;
ProgressWindow::ProgressWindow(HWND hwnd_parent):hwnd_parent_(hwnd_parent) {
  RECT rect;
  GetClientRect(hwnd_parent, &rect);
  RECT pb_rect;
  pb_rect.left = 10;
  pb_rect.top = (rect.bottom - rect.top) - 20 - kPBHeight;
  pb_rect.right = (rect.right - rect.left) - 10;
  pb_rect.bottom = pb_rect.top + kPBHeight;
  HMODULE hInstance = GetModuleHandle(NULL);
  InitCommonControls();
  hwnd_ = CreateWindowExW(0, PROGRESS_CLASS, L"",
    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | PBS_SMOOTH,
    pb_rect.left,
    pb_rect.top,
    pb_rect.right - pb_rect.left,
    pb_rect.bottom - pb_rect.top, hwnd_parent_, (HMENU)0, hInstance, nullptr);
}

void ProgressWindow::SetTopWindow(){
  BringWindowToTop(hwnd_);
}

void ProgressWindow::SetRange(int64_t media_duration) {
  media_duration_ = media_duration;
  SendMessage(hwnd_, PBM_SETRANGE, 0, MAKELPARAM(0, media_duration/1000));
}

void ProgressWindow::SetProgress(int64_t timestamp) {
  SendMessage(hwnd_, PBM_SETPOS, timestamp/1000, (LPARAM)0);
}

} // namespace mediakit