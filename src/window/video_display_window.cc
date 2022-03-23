#if 1
#include "video_display_window.h"

namespace mediakit {
const wchar_t *kWindowClassName = L"VideoDisplayWindow";
const wchar_t *kWindowTitle = L"mediakit";

VideoDisplayWindow::VideoDisplayWindow():is_initialized_(false) {
  hInstance_ = GetModuleHandle(NULL);
  RegisterWindowClass(hInstance_);
  gl_context_win_.reset(new media::GLContextWin());
  hwnd_ = CreateWindowW(kWindowClassName, kWindowTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr,
                        hInstance_, nullptr);
  SetWindowLong(
      hwnd_, GWL_EXSTYLE,
      GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
  ShowWindow(hwnd_, 10);
  UpdateWindow(hwnd_);
}

void VideoDisplayWindow::Initialize() {
  if(is_initialized_) return;
  is_initialized_ = true;
  gl_context_win_->Initialize(hwnd_, GetWindowDC(hwnd_));
  render_.Init();
  RECT rects;
  ::GetWindowRect(hwnd_, &rects);
  int w = rects.right - rects.left;
  int h = rects.bottom - rects.top;
  render_.SetViewport(w, h);
}

void VideoDisplayWindow::display(std::shared_ptr<media::VideoFrame> video_frame) {
  if(!video_frame) {
    return;
  }
  if(!is_initialized_) {
    Initialize();
  }
  RECT rects;
  ::GetWindowRect(hwnd_, &rects);
  int w = rects.right - rects.left;
  int h = rects.bottom - rects.top;
  render_.SetViewport(w, h);
  render_.updateTexture(video_frame);
  render_.render(w, h, video_frame->_w, video_frame->_h);
}

ATOM VideoDisplayWindow::RegisterWindowClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
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

LRESULT CALLBACK WndProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam) {
  switch (message) {
    case WM_PAINT: {} break;
    case WM_SIZE: {} break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

}  // namespace mediakit
#endif