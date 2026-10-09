#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/Logging.hpp>
#include <MyGameEngine/Win32Inc.hpp>

extern uint GameMain();

int __stdcall WinMain(::HINSTANCE, ::HINSTANCE, LPSTR, int) {
  Log::Info("Starting up...");
  return GameMain();
}
