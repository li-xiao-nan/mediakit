#include "ui/progress_window.h"

#include <CommCtrl.h >

namespace mediakit {
static const int kPBHeight = 8;
HHOOK ProgressWindow::pre_hook_ = 0;
HWND ProgressWindow::hwnd_ = 0;
ProgressWindow::ProgressWindow(HWND hwnd_parent, int left, int top, int w, int h)
  : hwnd_parent_(hwnd_parent) {
  RECT rect;
  GetClientRect(hwnd_parent, &rect);
  RECT pb_rect;
  pb_rect.left = left;
  pb_rect.top = top;
  pb_rect.right = left + w;
  pb_rect.bottom = top + h;
  HMODULE hInstance = GetModuleHandle(NULL);
  InitCommonControls();
  hwnd_ = CreateWindowExW(0, PROGRESS_CLASS, L"",
    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | PBS_SMOOTH,
    pb_rect.left,
    pb_rect.top,
    pb_rect.right - pb_rect.left,
    pb_rect.bottom - pb_rect.top, hwnd_parent_, (HMENU)0, hInstance, nullptr);
  // 获得线程ID
  DWORD dwThreadId = 0;
  dwThreadId = GetWindowThreadProcessId(hwnd_, NULL);
  pre_hook_ =
      SetWindowsHookEx(WH_GETMESSAGE, GetHookBrowerProc, NULL, dwThreadId);
}

LRESULT CALLBACK ProgressWindow::GetHookBrowerProc(int code,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  MSG* pMsg = (MSG*)lParam;
  RECT rect = {};
  if (nullptr == pMsg || code < 0 || pMsg->hwnd != hwnd_) {
    return CallNextHookEx(pre_hook_, code, wParam, lParam);
  }

  switch (pMsg->message) {
      case WM_LBUTTONUP: {
      } break;
      case WM_MOUSEMOVE: {
        GetClientRect(pMsg->hwnd, &rect);
      } break;
      case WM_LBUTTONDOWN: {
        GetClientRect(pMsg->hwnd, &rect);
      } break;
      case WM_MOUSELEAVE: {
      } break;
      case WM_POINTERUPDATE: {
      } break;
      case WM_POINTERDOWN: {
      } break;
      case WM_POINTERUP: {
      } break;
      case WM_POINTERENTER: {
      } break;
      case WM_POINTERLEAVE: {
      } break;
      default:
        break;
  }
  return CallNextHookEx(pre_hook_, code, wParam, lParam);
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

void ProgressWindow::UpdateWindowPosition(int left, int top, int w, int h) {
  SetWindowPos(hwnd_, 0, left, top, w, h, SWP_NOZORDER);
}

} // namespace mediakit