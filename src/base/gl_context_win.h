#ifndef BASE_GL_CONTEXT_WIN_H
#define BASE_GL_CONTEXT_WIN_H

#include <windows.h>

namespace media {
class GLContextWin {
 public:
   GLContextWin();
  ~GLContextWin() = default;
  void SetCurrentGLContext();
  void Initialize(HWND hwnd, HDC hdc);
private:
  void SetupPixelFormat(HDC);


 private:
  HWND hwnd_;
  HDC hdc_;
  HGLRC hrc_;
  int format_;
};
} // namespace media

#endif