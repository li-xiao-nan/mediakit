#include "ui/main_window.h"

namespace mediakit {
static const wchar_t* kWindowClassName = L"MainWindow";
static  const wchar_t* kWindowTitle = L"MediaKit";
static const int kPBHeight = 8;
static const int kPlayControlAreaHeight = 30;
static const int kPlayTimeTextControlWidth = 130;
static const int kPlayTimeTextControlHeight = 15;
static const int kPlayPauseButtionID = 100;
static const int kPlayPauseButtionWidth = 50;
static const int kPlayPauseButtionHeight = 20;

MainWindow::MainWindow() : pre_playing_timestamp_by_second_(0)
    , is_pauseing_(false){
    HMODULE hInstance = GetModuleHandle(NULL);
    RegisterWindowClass(hInstance);
    hwnd_ = CreateWindowW(kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr,
                          hInstance, nullptr);
    SetWindowLong(
        hwnd_, GWL_EXSTYLE,
        GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
    ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hwnd_, SW_SHOWNORMAL);
    UpdateWindow(hwnd_);
    RECT rect;
    GetClientRect(hwnd_, &rect);
    int parent_window_width = rect.right - rect.left;
    int parent_window_height = rect.bottom - rect.top;
    CreatePlayingTimeTextControl(
      10,
      rect.bottom  - kPlayControlAreaHeight + kPBHeight,
      kPlayTimeTextControlWidth,
      kPlayTimeTextControlHeight);
    CreatePlayPauseButtion(parent_window_width/2 - kPlayTimeTextControlWidth/2,
      rect.bottom - kPlayControlAreaHeight + kPBHeight,
      kPlayPauseButtionWidth,
      kPlayPauseButtionHeight);
    progress_window_.reset(
      new ProgressWindow(this, hwnd_, 0, 
        rect.bottom - kPlayControlAreaHeight, rect.right - rect.left, kPBHeight));

  }

LRESULT CALLBACK MainWindowProc(HWND hWnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
  MainWindow* main_window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if(!main_window) {
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  switch (message) {
  case WM_COMMAND:{
    int control_id = LOWORD(wParam);
    if (control_id == kPlayPauseButtionID) {
      main_window->OnPlayPauseButtionClick();
    }
    break;
  }
  case WM_PAINT: {
    } break;
  case WM_KEYUP:{
    // ¿Õ¸ñ¼ü
    if(wParam == 32) {
      main_window->OnPlayPauseButtionClick();
    }
    break;
  }
    case WM_SIZE: {
      main_window->OnWindowSizeChanged();
    } break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void MainWindow::OnWindowSizeChanged() {
  RECT rect;
  GetClientRect(hwnd_, &rect);
  int w = rect.right - rect.left;
  int h = rect.bottom - rect.top;
  if(mediaplayer_instance_) {
    mediaplayer_instance_->UpdatePlayerWindowPosition(0, 0, w, h-kPlayControlAreaHeight);
  }
  if(progress_window_) {
    progress_window_->UpdateWindowPosition(
      0, rect.bottom - kPlayControlAreaHeight, rect.right - rect.left, kPBHeight);
  }
  SetWindowPos(playing_time_hwnd_, 0, 10,
               rect.bottom - kPlayControlAreaHeight + kPBHeight,
               kPlayTimeTextControlWidth, kPlayTimeTextControlHeight, SWP_NOZORDER);
  SetWindowPos(play_pause_hwnd_, 0, w/2 - kPlayTimeTextControlWidth/2,
                rect.bottom - kPlayControlAreaHeight + kPBHeight,
                kPlayPauseButtionWidth, kPlayPauseButtionHeight, SWP_NOZORDER);
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

void MainWindow::SetMediaPlayer(std::shared_ptr<MediaPlayer> mediaplayer){
  mediaplayer_instance_ = mediaplayer;
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

void MainWindow::Seek(int timestamp_ms) {
  if(!mediaplayer_instance_) return;
  mediaplayer_instance_->Seek(timestamp_ms);
}

void MainWindow::CreatePlayPauseButtion(int left, int top, int w, int h) {
  play_pause_hwnd_ = CreateWindow(
      L"BUTTON",          // Predefined class; Unicode assumed
      L"ÔÝÍ£",  // Button text
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
      left,                                                     // x position
      top,                                                    // y position
      w,                                                    // Button width
      h,                                                     // Button height
      hwnd_,                                                   // Parent window
      (HMENU)kPlayPauseButtionID,  // Assign appropriate control ID
      (HINSTANCE)GetWindowLong(hwnd_, GWL_HINSTANCE),
      NULL);  // Pointer not needed.
}

void MainWindow::OnPlayPauseButtionClick(){
  if(is_pauseing_) {
    is_pauseing_ = false;
    mediaplayer_instance_->Resume();
    SetWindowText(play_pause_hwnd_ ,L"ÔÝÍ£");
  } else {
    is_pauseing_ = true;
    mediaplayer_instance_->Pause();
    SetWindowText(play_pause_hwnd_, L"²¥·Å");
  }
}

} // namespace mediakit