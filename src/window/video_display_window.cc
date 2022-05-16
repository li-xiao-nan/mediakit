#if 1
#include "video_display_window.h"

namespace mediakit {
static const wchar_t *kWindowClassName = L"VideoDisplayWindow";
static const wchar_t *kWindowTitle = L"mediakit";

VideoDisplayWindow::VideoDisplayWindow(HWND parent_hwnd, int x, int y, int w, int h):
  is_initialized_(false),
  parent_hwnd_(parent_hwnd){
  hInstance_ = GetModuleHandle(NULL);
  RegisterWindowClass(hInstance_);
  gl_context_win_.reset(new media::GLContextWin());
  hwnd_ = CreateWindowW(kWindowClassName, kWindowTitle, WS_CHILD | WS_CLIPSIBLINGS,
                        x, y, w, h, parent_hwnd_, nullptr,
                        hInstance_, nullptr);
  //SetWindowLong(
  //    hwnd_, GWL_EXSTYLE,
  //    GetWindowLong(hwnd_, GWL_EXSTYLE) | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
  ShowWindow(hwnd_, SW_SHOWNORMAL);
  //UpdateWindow(hwnd_);
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

void VideoDisplayWindow::SetWindowPosition(int left, int top, int w, int h) {
  ::SetWindowPos(hwnd_, 0, left, top, w, h, SWP_NOZORDER);
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

void VideoDisplayWindow::CloseWindow() {
  ShowWindow(hwnd_, SW_HIDE);
  ::DestroyWindow(hwnd_);
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
  wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 0));// (HBRUSH)(COLOR_WINDOW + 1);
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
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

}  // namespace mediakit
#endif