#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H
#include <memory>
#include "player/media_player_client.h"
// 解决：#error:  WinSock.h has already been included
// Windows.h头文件，而Windows.h头文件又包含了WinSock.h这个老东西
// 1、在包含Windows.h前定义宏WIN32_LEAN_AND_MEAN，就OK了（WIN32_LEAN_AND_MEAN表示不包含一些极少使用和偏门的资料）
// 2、在包含Windows.h前包含winsock2.h
// 3、在包含Windows.h前包含asio.hpp
#include "ui/progress_window.h"
#include <windows.h>

namespace mediakit {

LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
class MainWindow : public MediaPlayerClient {
public:
  MainWindow();
  ~MainWindow() = default;
  HWND GetWindowHandle();
  void SetProgressWindowTop();

  // MediaPlayerClient impl
  void OnGetMediaInfo(const media::MediaInfo& media_info) override;
  void OnPlayStateChanged() override {};
  void OnPlayProgressUpdate(const int64_t timestamp) override;
private:
  ATOM RegisterWindowClass(HINSTANCE hInstance);
  void CreatePlayingTimeTextControl(int left, int top, int w, int h);
  std::wstring  FormatDurationInfo(int media_duration);
  void UpdatePlaytingShowText();
private:
  HWND hwnd_;
  HWND playing_time_hwnd_;
  std::wstring duration_format_str_;
  std::wstring current_playing_timestamp_str_;
  media::MediaInfo media_info_;
  std::unique_ptr<ProgressWindow> progress_window_;
  int pre_playing_timestamp_by_second_;
};
} // namespace mediakit

#endif