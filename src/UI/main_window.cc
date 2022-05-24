#include "ui/main_window.h"
#include <Shlobj.h>
#include <commdlg.h>
#include "log/log_wrapper.h"
#include "base/message_loop_thread_manager.h"

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
static const int kPreviewVideoWindowW = 300;
static const int kPreviewVideoWindowH = 150;

MainWindow::MainWindow()
  : pre_playing_timestamp_by_second_(0)
  , is_pauseing_(false)
  , pre_video_preview_timestamp_(0){
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
    CreateMainMenu(hwnd_);
    video_preview_window_.reset(new VideoPreviewWindow(
      hwnd_, 0, 0, kPreviewVideoWindowW, kPreviewVideoWindowH));
  }

MainWindow::~MainWindow() {
  LOGGING(media::LOG_LEVEL_INFO)<<"loging content";
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
    } else if(control_id == IDM_FILE_OPEN) {
      main_window->OnOpenNewFile();
    }
    break;
  }
  case WM_PAINT: {
    } break;
  case WM_KEYUP:{
    // 空格键
    if(wParam == 32) {
      main_window->OnPlayPauseButtionClick();
    }
    break;
  }
    case WM_SIZE: {
      main_window->OnWindowSizeChanged();
    } break;
    case WM_DESTROY:
      main_window->OnWindowClose();
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void MainWindow::OnWindowClose() {
  MediaPlayer::StopSystem();
  if(mediaplayer_instance_) {
    mediaplayer_instance_->Stop();
  }
}

void MainWindow::OnOpenNewFile() {
  OPENFILENAME ofn = {0};
  TCHAR strFilename[MAX_PATH] = {0};       //用于接收文件名
  ofn.lStructSize = sizeof(OPENFILENAME);  //结构体大小
  ofn.hwndOwner =
      NULL;  //拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
  ofn.lpstrFilter =
      TEXT("所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");  //设置过滤
  ofn.nFilterIndex = 1;                                      //过滤器索引
  ofn.lpstrFile = strFilename;  //接收返回的文件名，注意第一个字符需要为NULL
  ofn.nMaxFile = sizeof(strFilename);       //缓冲区长度
  ofn.lpstrInitialDir = NULL;               //初始目录为默认
  ofn.lpstrTitle = TEXT("请选择一个文件");  //使用系统默认标题留空即可
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
              OFN_HIDEREADONLY;  //文件、目录必须存在，隐藏只读选项
  if (GetOpenFileName(&ofn)){
    StartPlayNewVideo(strFilename);
  }
}

void MainWindow::DestroyPreMediaPlayer(std::shared_ptr<MediaPlayer> instance) {
  // 此函数主要作用在于，通过参数std::shared_ptr<MediaPlayer> instance，增加MediaPlayer对象引用技术，并通过posttask，
  // 将其发送至主线程消息队列，保证其在主线程任务队列中，完成销毁动作
  // 原因为:VideoRenderer中渲染线程，会定时将播放时钟同步状态通知给MediaPlayer，实现方式为发送至主线程任务队列，如果直接销毁MediaPlayer对象，
  // 会导致访问的对象，已经被释放掉
}

void MainWindow::StartPlayNewVideo(std::wstring file_path) {
  LOGGING(media::LOG_LEVEL_INFO)<<"StartPlayNewVideo";
  if(mediaplayer_instance_) {
    mediaplayer_instance_->Stop();
    LOGGING(media::LOG_LEVEL_INFO) << "mediaplayer_instance_->Stop()";
    POSTTASK(media::TID_MAIN, 
      boost::bind(
        &MainWindow::DestroyPreMediaPlayer, this, mediaplayer_instance_));
    mediaplayer_instance_ = nullptr;
  }
   RECT rects;
   ::GetClientRect(hwnd_, &rects);
   int w = rects.right - rects.left;
   int h = rects.bottom - rects.top;
   std::string video_url = media::UTF16toANSI(file_path);
   std::shared_ptr<mediakit::MediaPlayer> mediaplayer =
     mediakit::MediaPlayer::CreateMediaPlayer(hwnd_, 0, 0, w, h-30, video_url);

   SetMediaPlayer(mediaplayer);
   mediaplayer->SetClient(this);
   mediaplayer->Start();
   mediaplayer->SetPlaybackRate(1.0f);
   file_path_ = video_url;
   SetWindowText(hwnd_, file_path.c_str());
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
      L"暂停",  // Button text
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

void MainWindow::CreateMainMenu(HWND hwnd) {
  h_main_menu = CreateMenu();
  AppendMenu(h_main_menu, MF_STRING, IDM_FILE_OPEN, TEXT("打开文件"));
  SetMenu(hwnd, h_main_menu);
}

void MainWindow::OnPlayPauseButtionClick(){
  if(!mediaplayer_instance_) {
    return;
  }
  if(is_pauseing_) {
    is_pauseing_ = false;
    mediaplayer_instance_->Resume();
    SetWindowText(play_pause_hwnd_ ,L"暂停");
  } else {
    is_pauseing_ = true;
    mediaplayer_instance_->Pause();
    SetWindowText(play_pause_hwnd_, L"播放");
  }
}

void MainWindow::NotifyShowVideoPreview(int x, int y, int timestamp_ms) {
  if(!mediaplayer_instance_) return;
  if(timestamp_ms == pre_video_preview_timestamp_) {
    LOGGING(media::LOG_LEVEL_DEBUG) << "drop video preview request";
    return;
  } else {
    pre_video_preview_timestamp_= timestamp_ms;
  }
  mediaplayer_instance_->GetVideoKeyFrameAsync(
    timestamp_ms, kPreviewVideoWindowW, kPreviewVideoWindowH);
  ShowPreviewWindow(x, y);
}

void MainWindow::ShowPreviewWindow(int x, int y) {
  int preview_window_x = x;
  int preview_window_y = y - kPreviewVideoWindowH;
  video_preview_window_->NotifyShow(preview_window_x, preview_window_y, 
    kPreviewVideoWindowW, kPreviewVideoWindowH);
}

void MainWindow::HidePreviewWindow() {
  video_preview_window_->HideWindow();
}

void MainWindow::OnGetKeyVideoFrame(
    int timestamp_ms,
    std::shared_ptr<media::VideoFrame> video_frame) {
  video_preview_window_->OnReceiveVideoFrame(video_frame);
}

void MainWindow::OnOpenMediaFileFailed(const std::string file_name,
                           int error_code,
                           const std::string& error_description) {
  std::string content = "打开文件（"+ file_name + ")失败，error-code:" + std::to_string(error_code);
  MessageBoxA(hwnd_, content.c_str(), "播放失败", MB_OK);
}

} // namespace mediakit