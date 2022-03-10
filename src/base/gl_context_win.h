#ifndef BASE_GL_CONTEXT_WIN_H
#define BASE_GL_CONTEXT_WIN_H

#include <windows.h>

namespace media {
class GLContextWin {
 public:
   GLContextWin(HWND, HDC);
  ~GLContextWin() = default;
  void SetCurrentGLContext();
private:
  void Initialize(HWND, HDC);
  void SetupPixelFormat(HDC);


 private:
  HWND hwnd_;
  HDC hdc_;
  HGLRC hrc_;
  int format_;
};
} // namespace media

#endif