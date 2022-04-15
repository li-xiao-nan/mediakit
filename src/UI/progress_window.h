#ifndef UI_PROGRESS_WINDOW_H
#define UI_PROGRESS_WINDOW_H
#include <windows.h>
#include <iostream>
namespace mediakit {

class ProgressWindow {
public:
  ProgressWindow(HWND hwnd_parent, int left, int top, int w, int h);
  ~ProgressWindow() = default;
  void SetTopWindow();
  // unit:ms
  void SetRange(int64_t media_duration);
  void SetProgress(int64_t timestamp);
private:
  static LRESULT CALLBACK GetHookBrowerProc(int code,
                                            WPARAM wParam,
                                            LPARAM lParam);
 private:
  static HHOOK pre_hook_;
  int64_t media_duration_;
  HWND hwnd_parent_;
  static HWND hwnd_;
};
} // namespace mediakit

#endif