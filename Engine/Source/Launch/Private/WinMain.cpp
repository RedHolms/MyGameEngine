#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/win32api/Base.hpp>

extern uint GameMain();

int __stdcall WinMain(win32api::HINSTANCE, win32api::HINSTANCE, const char*, int) {
  return GameMain();
}
