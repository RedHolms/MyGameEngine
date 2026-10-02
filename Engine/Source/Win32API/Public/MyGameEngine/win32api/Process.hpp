#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess
HANDLE GetCurrentProcess(void) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocessid
DWORD GetCurrentProcessId(void) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess
[[noreturn]] void ExitProcess(UINT exitCode) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
