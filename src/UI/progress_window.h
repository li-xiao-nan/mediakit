#ifndef UI_PROGRESS_WINDOW_H
#define UI_PROGRESS_WINDOW_H
#include <windows.h>
#include <iostream>
namespace mediakit {
class MainWindow;
class ProgressWindow {
public:
  ProgressWindow(MainWindow* mian_window, HWND hwnd_parent, int left, int top, int w, int h);
  ~ProgressWindow() = default;
  void SetTopWindow();
  // unit:ms
  void SetRange(int64_t media_duration);
  void SetProgress(int64_t timestamp);
  void UpdateWindowPosition(int left, int top, int w, int h);
  void ShowHoverWindow(int x, int y);
  void HideHoverWindow();
  void OnLButtionDown(int x, int y);
private:
  static LRESULT CALLBACK GetHookBrowerProc(int code,
                                            WPARAM wParam,
                                            LPARAM lParam);
  void CreateHoverTextControl();
  int CaculatePlayTimestampByXPos(int x);
 private:
  int left_, top_, width_, height_;
  static HHOOK pre_hook_;
  int64_t media_duration_;
  HWND hwnd_parent_;
  static HWND hwnd_;
  HWND hwnd_hover_;
  MainWindow* main_window_;
};
} // namespace mediakit

#endif