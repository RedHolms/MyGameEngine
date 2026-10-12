#include <MyGameEngine/Console.hpp>

#include <cstdio>
#include <iostream>

#include <Windows.h>

void Console::MakeSureConsolePresent() {
  BOOL attached = AttachConsole(ATTACH_PARENT_PROCESS);

  if (!attached) {
    AllocConsole();

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    DWORD consoleMode;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(output, &consoleMode);
    SetConsoleMode(output, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
  }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations" // We can safely use "freopen" here
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
#pragma clang diagnostic pop
  std::ios::sync_with_stdio(true);
}
