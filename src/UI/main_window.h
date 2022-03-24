#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H
#include <windows.h>

namespace mediakit {

LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
class MainWindow {
public:
  MainWindow();
  ~MainWindow() = default;
  HWND GetWindowHandle();
private:
  ATOM RegisterWindowClass(HINSTANCE hInstance);
private:
  HWND hwnd_;
};
} // namespace mediakit

#endif