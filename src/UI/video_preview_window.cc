#include "UI/video_preview_window.h"
#include <string>

namespace mediakit {
static const wchar_t* const kVideoPreviewWindowClassName = L"VideoPreviewWindow";
static const wchar_t* const kVideoPreviewWindowTitle = L"Video Preview";
VideoPreviewWindow::VideoPreviewWindow(::HWND hwnd, int x, int y,
  int w, int h) : parent_hwnd_(hwnd), x_(x), y_(y), w_(w), h_(h){
  hInstance_ = GetModuleHandle(NULL);
  RegisterWindowClass(hInstance_);
  hwnd_ = CreateWindowW(kVideoPreviewWindowClassName, kVideoPreviewWindowTitle,
    WS_CHILD | WS_CLIPSIBLINGS,
    x, y, w, h, parent_hwnd_, nullptr, hInstance_, nullptr);

  ::SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  InitBigMapInfo();
  ::ShowWindow(hwnd_, SW_HIDE);
}

LRESULT CALLBACK VideoPreviewProc(
  ::HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  VideoPreviewWindow* video_preview_window = 
    reinterpret_cast<VideoPreviewWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  PAINTSTRUCT ps;
  HDC hdcClient, hdcWindow;
  switch (message) {
    case WM_PAINT: {
      BeginPaint(hWnd, &ps);
      video_preview_window->DrawVideoFrame();
      EndPaint(hWnd, &ps);
    } break;
    case WM_SIZE: {
    } break;
    case WM_DESTROY:
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

void VideoPreviewWindow::DrawVideoFrame() {
  if(!video_frame_) return;
  RECT rect;
  GetClientRect(hwnd_, &rect);  // pWnd指向CWnd类的一个指针
  HDC hDC = GetDC(hwnd_);  // HDC是Windows的一种数据类型，是设备描述句柄；
  SetStretchBltMode(hDC, COLORONCOLOR);
  StretchDIBits(hDC, 0, 0, 
    rect.right,   //显示窗口宽度
    rect.bottom,  //显示窗口高度
    0, 0,
    video_frame_->_w,   //图像宽度
    video_frame_->_h,  //图像高度
    video_frame_->_data,
    bit_map_info_,
    DIB_PAL_COLORS,
    SRCCOPY);
  ReleaseDC(hwnd_, hDC);
}

void VideoPreviewWindow::InitBigMapInfo() {
  unsigned char* bit_buffer = new unsigned char[40 + 4 * 256];  //开辟一个内存区域

  if (bit_buffer == NULL) {
    return;
  }

  memset(bit_buffer, 0, 40 + 4 * 256);
  bit_map_info_ = (BITMAPINFO*)bit_buffer;
  bit_map_info_->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bit_map_info_->bmiHeader.biPlanes = 1;
  for (int i = 0; i < 256; i++) {  //颜色的取值范围 (0-255)
    bit_map_info_->bmiColors[i].rgbBlue = (unsigned char)i; 
    bit_map_info_->bmiColors[i].rgbGreen = (unsigned char)i;
    bit_map_info_->bmiColors[i].rgbRed = (unsigned char)i;
  }
  bit_map_info_->bmiHeader.biHeight = -h_;
  bit_map_info_->bmiHeader.biWidth = w_;
  bit_map_info_->bmiHeader.biBitCount = 3 * 8;
}

void VideoPreviewWindow::OnReceiveVideoFrame(
  std::shared_ptr<media::VideoFrame> video_frame) {
  video_frame_ = video_frame;
  DrawVideoFrame();
  if(pending_show_window_) {
    ShowWindow(x_, y_, w_, h_);
  }
}

ATOM VideoPreviewWindow::RegisterWindowClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = VideoPreviewProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(109));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground =
      CreateSolidBrush(RGB(255, 255, 255));  // (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(109);
  wcex.lpszClassName = kVideoPreviewWindowClassName;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(108));

  return RegisterClassExW(&wcex);
}

void VideoPreviewWindow::NotifyShow(int x, int y, int w, int h) {
  x_ = x;
  y_ = y;
  w_ = w;
  h_ = h;
  pending_show_window_ = true;
}

void VideoPreviewWindow::ShowWindow(int x, int y, int w, int h) {
  ::SetWindowPos(hwnd_, 0, x, y, w, h, SWP_NOZORDER);
  ::ShowWindow(hwnd_, SW_SHOWNORMAL);
}

void VideoPreviewWindow::HideWindow() {
  pending_show_window_ = false;
  ::ShowWindow(hwnd_, SW_HIDE);
}

} // namespace mediakit