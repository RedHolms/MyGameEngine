#include <MyGameEngine/IntTypes.hpp>

#include <MyGameEngine/win32api/base.hpp>

extern uint GameMain();

int __stdcall WinMain(::HINSTANCE, ::HINSTANCE, LPSTR, int) {
  return GameMain();
}
