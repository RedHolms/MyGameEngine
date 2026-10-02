#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
DWORD GetLastError(void) _WIN32_FUNC_ATTRS;

constexpr DWORD EXCEPTION_NONCONTINUABLE = 0x1;

// https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-raiseexception
void RaiseException(DWORD code, DWORD flags, DWORD argsCount, ULONG_PTR const* args) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
