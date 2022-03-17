#include <windows.h>
#include "window/main_message_loop_win.h"

namespace mediakit {
int EnterMainMessageLoop(HINSTANCE hInstance) {
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(109));
  MSG msg;
  // 主消息循环
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}
} //namespace mediakit