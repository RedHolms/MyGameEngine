#include "Memory/UniquePtr.hpp"
#include "Threads/CurrentThread.hpp"
#include "Window.hpp"

uint GameMain() {
  auto gameWindow = MakeUnique<Window>("Hello"s, 1280, 720);

  while (true)
    CurrentThread::Yield();

  return 0;
}
