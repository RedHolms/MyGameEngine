#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/console/allocconsole
BOOL AllocConsole(void) _WIN32_FUNC_ATTRS;

constexpr DWORD ATTACH_PARENT_PROCESS = (DWORD)-1;

// https://learn.microsoft.com/en-us/windows/console/attachconsole
BOOL AttachConsole(DWORD processId) _WIN32_FUNC_ATTRS;

constexpr DWORD STD_INPUT_HANDLE = (DWORD)-10;
constexpr DWORD STD_OUTPUT_HANDLE = (DWORD)-11;
constexpr DWORD STD_ERROR_HANDLE = (DWORD)-12;

// https://learn.microsoft.com/en-us/windows/console/getstdhandle
HANDLE GetStdHandle(DWORD handleType) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/console/writeconsole
BOOL WriteConsoleW(HANDLE consoleOutput, const wchar_t* string, DWORD charactersToWrite, LPDWORD outCharactersWritten,
  LPVOID __reserved_must_be_nullptr) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
