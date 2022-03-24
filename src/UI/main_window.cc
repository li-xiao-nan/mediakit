#include "ui/main_window.h"

namespace mediakit {
static const wchar_t* kWindowClassName = L"MainWindow";
static  const wchar_t* kWindowTitle = L"MediaKit";

MainWindow::MainWindow() {
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

} // namespace mediakit