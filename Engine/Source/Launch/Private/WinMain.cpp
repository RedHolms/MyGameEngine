#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/Win32Inc.hpp>

extern uint GameMain();

int __stdcall WinMain(::HINSTANCE, ::HINSTANCE, LPSTR, int) {
  return GameMain();
}
