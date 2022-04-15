#include "ui/main_window.h"

namespace mediakit {
static const wchar_t* kWindowClassName = L"MainWindow";
static  const wchar_t* kWindowTitle = L"MediaKit";
static const int kPBHeight = 8;
static const int kPlayControlAreaHeight = 30;
static const int kPlayTimeTextControlWidth = 130;
static const int kPlayTimeTextControlHeight = 15;

MainWindow::MainWindow() : pre_playing_timestamp_by_second_(0) {
    HMODULE hInstance = GetModuleHandle(NULL);
    RegisterWindowClass(hInstance);
    hwnd_ = CreateWindowW(kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr,
                          hInstance, nullptr);
    SetWindowLong(
        hwnd_, GWL_EXSTYLE,
        GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    ShowWindow(hwnd_, SW_SHOWNORMAL);
    UpdateWindow(hwnd_);
    RECT rect;
    GetClientRect(hwnd_, &rect);
    CreatePlayingTimeTextControl(
      10,
      rect.bottom  - kPlayControlAreaHeight + kPBHeight,
      kPlayTimeTextControlWidth,
      kPlayTimeTextControlHeight);
    progress_window_.reset(
      new ProgressWindow(hwnd_, 0, rect.bottom - kPlayControlAreaHeight, rect.right - rect.left, kPBHeight));
  }
LRESULT CALLBACK MainWindowProc(HWND hWnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (message) {
      case WM_PAINT: {
      } break;
      case WM_SIZE: {
      } break;
      case WM_DESTROY:
        PostQuitMessage(0);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM MainWindow::RegisterWindowClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MainWindowProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(109));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(109);
  wcex.lpszClassName = kWindowClassName;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108));

  return RegisterClassExW(&wcex);
}

HWND MainWindow::GetWindowHandle() {
  return hwnd_;
}

void MainWindow::SetProgressWindowTop() {
  progress_window_->SetTopWindow();
}

void MainWindow::OnGetMediaInfo(const media::MediaInfo& media_info) {
  media_info_.video_duration_ = media_info.video_duration_;
  if(progress_window_) {
    progress_window_->SetRange(media_info_.video_duration_);
  }
  duration_format_str_ = FormatDurationInfo(media_info_.video_duration_);
  UpdatePlaytingShowText();
}

void MainWindow::UpdatePlaytingShowText(){
  std::wstring playting_show_text = current_playing_timestamp_str_ + L" / " + duration_format_str_;
  SetWindowText(playing_time_hwnd_, playting_show_text.c_str());
}

void MainWindow::OnPlayProgressUpdate(const int64_t timestamp) {
  if(progress_window_) {
    progress_window_->SetProgress(timestamp);
  }
  if (pre_playing_timestamp_by_second_ != timestamp / 1000) {
    pre_playing_timestamp_by_second_ = timestamp / 1000;
    current_playing_timestamp_str_ = FormatDurationInfo(timestamp);
    UpdatePlaytingShowText();
  }
}

template <typename... Args>
std::string string_format(const std::string& format, Args... args) {
  size_t size =
      1 + snprintf(nullptr, 0, format.c_str(), args...);  // Extra space for \0
  // unique_ptr<char[]> buf(new char[size]);
  char* bytes = new char[size];
  snprintf(bytes, size, format.c_str(), args...);
  std::string result = std::string(bytes);
  delete [] bytes;
  return result;
}

template <typename... Args>
std::wstring wstring_format(const std::wstring& format, Args... args) {
  size_t size =
      1 + swprintf(nullptr, 0, format.c_str(), args...);  // Extra space for \0
  // unique_ptr<char[]> buf(new char[size]);
  wchar_t bytes[10];
  swprintf(bytes, size, format.c_str(), args...);
  std::wstring result = bytes;
  return result;
}


std::wstring  MainWindow::FormatDurationInfo(int media_duration) {
  int duration_by_second = media_duration / 1000;
  int hour_value = duration_by_second / (60*60);
  int minute_value = duration_by_second / 60 % 60;
  int second_value = duration_by_second % 60;
  std::wstring hour_value_str = hour_value>=10 ? std::to_wstring(hour_value):std::to_wstring(0) + std::to_wstring(hour_value);
  std::wstring minute_value_str = minute_value >= 10 ? std::to_wstring(minute_value) : std::to_wstring(0) + std::to_wstring(minute_value);
  std::wstring second_value_str = second_value >= 10 ? std::to_wstring(second_value) : std::to_wstring(0) + std::to_wstring(second_value);

  std::wstring result = hour_value_str + L":" + minute_value_str + L":" + second_value_str;
  return result;
}

void MainWindow::CreatePlayingTimeTextControl(int left, int top, int w, int h) {
  HMODULE hInstance = GetModuleHandle(NULL);
  playing_time_hwnd_ = CreateWindow(
      L"static",                      
      L"PlayingTimeWindow",
      WS_CHILD | WS_VISIBLE ,  
      left, top, w, h,
      hwnd_,                                  
      (HMENU)2,                              
      hInstance, NULL);

  current_playing_timestamp_str_ = FormatDurationInfo(0);
  duration_format_str_ = FormatDurationInfo(0);
  UpdatePlaytingShowText();
}

} // namespace mediakit