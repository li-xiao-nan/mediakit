#ifndef UI_PROGRESS_WINDOW_H
#define UI_PROGRESS_WINDOW_H
#include <windows.h>
#include <iostream>
namespace mediakit {

class ProgressWindow {
public:
  ProgressWindow(HWND hwnd_parent);
  ~ProgressWindow() = default;
  void SetTopWindow();
  // unit:ms
  void SetRange(int64_t media_duration);
  void SetProgress(int64_t timestamp);
 private:
  static HWND hwnd_;
  int64_t media_duration_;
  HWND hwnd_parent_;

};
} // namespace mediakit

#endif