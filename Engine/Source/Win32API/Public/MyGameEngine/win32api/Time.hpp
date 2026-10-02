#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

typedef struct FILETIME {
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} *PFILETIME, *LPFILETIME;

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimeasfiletime
void GetSystemTimeAsFileTime(LPFILETIME outTime) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount
DWORD GetTickCount(void) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount64
ULONGLONG GetTickCount64(void) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
