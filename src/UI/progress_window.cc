#include "ui/progress_window.h"
#include <string>
#include <CommCtrl.h >
#include "ui/main_window.h"
#include "log/log_wrapper.h"

namespace mediakit {
static int kHoverTextControlWidth = 65;
static int kHoverTextControlHeight = 15;
HHOOK ProgressWindow::pre_hook_ = 0;
HWND ProgressWindow::hwnd_ = 0;
ProgressWindow::ProgressWindow(MainWindow* main_window,
  HWND hwnd_parent, int left, int top, int w, int h)
  : main_window_(main_window)
  , hwnd_parent_(hwnd_parent)
  , left_(left)
  , top_(top)
  , width_(w)
  , height_(h){
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
  ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  CreateHoverTextControl();
}

LRESULT CALLBACK ProgressWindow::GetHookBrowerProc(int code,
                                          WPARAM wParam,
                                          LPARAM lParam) {
  MSG* pMsg = (MSG*)lParam;
  RECT rect = {};
  static bool hover_showing = false;
  if (nullptr == pMsg || code < 0 ) {
    return CallNextHookEx(pre_hook_, code, wParam, lParam);
  }
  ProgressWindow* progress_window = reinterpret_cast<ProgressWindow*>(GetWindowLongPtr(hwnd_, GWLP_USERDATA));
  if (!progress_window) {
    return CallNextHookEx(pre_hook_, code, wParam, lParam);
  }

  if (pMsg->hwnd != hwnd_) {
    if(pMsg->message == WM_MOUSEMOVE && hover_showing) {
      progress_window->HideHoverWindow();
      hover_showing = false;
    }
    return CallNextHookEx(pre_hook_, code, wParam, lParam);
  }

  switch (pMsg->message) {
      case WM_LBUTTONUP: {
      } break;
      case WM_MOUSEMOVE: {
        int x, y;
        x = LOWORD(pMsg->lParam);
        y = HIWORD(pMsg->lParam);
        progress_window->ShowHoverWindow(x, y);
        hover_showing = true;
      } break;
      case WM_LBUTTONDOWN: {
        int x, y;
        x = LOWORD(pMsg->lParam);
        y = HIWORD(pMsg->lParam);
        progress_window->OnLButtionDown(x, y);
      } break;
      case WM_NCMOUSELEAVE: {
        progress_window->HideHoverWindow();
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
  left_ = left;
  top_ = top;
  width_ = w;
  height_ = h;
  SetWindowPos(hwnd_, 0, left, top, w, h, SWP_NOZORDER);
}

void ProgressWindow::CreateHoverTextControl() {
  HMODULE hInstance = GetModuleHandle(NULL);
  hwnd_hover_ =
      CreateWindow(L"static", L"01:22:23", WS_CHILD | WS_CLIPSIBLINGS, 0,
        0, kHoverTextControlWidth, kHoverTextControlHeight, 
        hwnd_parent_, (HMENU)2, hInstance, NULL);
}

std::wstring FormatDurationInfoW(int media_duration) {
  int duration_by_second = media_duration / 1000;
  int hour_value = duration_by_second / (60 * 60);
  int minute_value = duration_by_second / 60 % 60;
  int second_value = duration_by_second % 60;
  std::wstring hour_value_str =
      hour_value >= 10 ? std::to_wstring(hour_value)
                       : std::to_wstring(0) + std::to_wstring(hour_value);
  std::wstring minute_value_str =
      minute_value >= 10 ? std::to_wstring(minute_value)
                         : std::to_wstring(0) + std::to_wstring(minute_value);
  std::wstring second_value_str =
      second_value >= 10 ? std::to_wstring(second_value)
                         : std::to_wstring(0) + std::to_wstring(second_value);

  std::wstring result =
      hour_value_str + L":" + minute_value_str + L":" + second_value_str;
  return result;
}

std::string FormatDurationInfoA(int media_duration) {
  int duration_by_second = media_duration / 1000;
  int hour_value = duration_by_second / (60 * 60);
  int minute_value = duration_by_second / 60 % 60;
  int second_value = duration_by_second % 60;
  std::string hour_value_str =
      hour_value >= 10 ? std::to_string(hour_value)
                       : std::to_string(0) + std::to_string(hour_value);
  std::string minute_value_str =
      minute_value >= 10 ? std::to_string(minute_value)
                         : std::to_string(0) + std::to_string(minute_value);
  std::string second_value_str =
      second_value >= 10 ? std::to_string(second_value)
                         : std::to_string(0) + std::to_string(second_value);

  std::string result =
      hour_value_str + ":" + minute_value_str + ":" + second_value_str;
  return result;
}

void ProgressWindow::ShowHoverWindow(int x, int y) {
  if(x == pre_x_ && y == pre_y_) {
    return;
  }
  pre_x_ = x;
  pre_y_ = y;
  int hover_window_x = left_ + x;
  int hover_window_y = top_ - kHoverTextControlHeight;
  ::SetWindowPos(hwnd_hover_, 0, hover_window_x, hover_window_y, kHoverTextControlWidth,
    kHoverTextControlHeight, SWP_NOZORDER);
  int hover_timestamp = CaculatePlayTimestampByXPos(x);
  main_window_->NotifyShowVideoPreview(hover_window_x, hover_window_y, hover_timestamp);
  SetWindowText(hwnd_hover_, FormatDurationInfoW(hover_timestamp).c_str());
  ShowWindow(hwnd_hover_, SW_SHOWNORMAL);
}

int ProgressWindow::CaculatePlayTimestampByXPos(int x) {
  float progress_value = x * 1.0 / width_ * 1.0f;
  int hover_timestamp = media_duration_ * progress_value;
  return hover_timestamp;
}

void ProgressWindow::OnLButtionDown(int x, int y) {
  int click_timestamp = CaculatePlayTimestampByXPos(x);
  main_window_->Seek(click_timestamp);
  LOGGING(media::LogLevel::LOG_LEVEL_INFO)<<"seek:"<<FormatDurationInfoA(click_timestamp).c_str()<<"("<<click_timestamp<<"ms)";
}

void ProgressWindow::HideHoverWindow() {
  ShowWindow(hwnd_hover_, SW_HIDE);
  main_window_->HidePreviewWindow();
}

} // namespace mediakit