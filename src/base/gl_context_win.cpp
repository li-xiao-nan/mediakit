#include "base/gl_context_win.h"
extern "C" {
#include "GL/glew.h"
}

namespace media {
GLContextWin::GLContextWin(HWND hwnd, HDC hdc): 
  hwnd_(hwnd), hdc_(hdc), hrc_(0), format_(0) {
  Initialize(hwnd_, hdc_);
}

void GLContextWin::SetupPixelFormat(HDC hdc) {
  int pixelFormat;
  PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),  // size
      1,                              // version
      PFD_SUPPORT_OPENGL |            // OpenGL window
          PFD_DRAW_TO_WINDOW |        // render to window
          //          PFD_DOUBLEBUFFER,           // support double-buffering
          PFD_TYPE_RGBA,  // color type
      32,                 // prefered color depth
      0,
      0,
      0,
      0,
      0,
      0,  // color bits (ignored)
      0,  // no alpha buffer
      0,  // alpha bits (ignored)
      0,  // no accumulation buffer
      0,
      0,
      0,
      0,               // accum bits (ignored)
      16,              // depth buffer
      0,               // no stencil buffer
      0,               // no auxiliary buffers
      PFD_MAIN_PLANE,  // main layer
      0,               // reserved
      0,
      0,
      0,  // no layer, visible, damage masks
  };

  pixelFormat = ChoosePixelFormat(hdc_, &pfd);
  SetPixelFormat(hdc_, pixelFormat, &pfd);
}

void GLContextWin::Initialize(HWND hwnd, HDC hdc) {
  hwnd_ = hwnd;
  hdc_ = hdc;
  SetupPixelFormat(hdc_);
  hrc_ = wglCreateContext(hdc_);
  wglMakeCurrent(hdc_, hrc_);
  glewExperimental = GL_TRUE;
  glewInit();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
}

void GLContextWin::SetCurrentGLContext() {
  wglMakeCurrent(hdc_, hrc_);  //执行OpenGL指令列表中的指令
}

} // namespace media