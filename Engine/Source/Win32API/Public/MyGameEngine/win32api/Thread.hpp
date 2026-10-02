#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthread
HANDLE GetCurrentThread(void) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid
DWORD GetCurrentThreadId(void) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-switchtothread
BOOL SwitchToThread(void) _WIN32_FUNC_ATTRS;

using LPTHREAD_START_ROUTINE = DWORD(__stdcall*)(void*);

constexpr size_t STACK_SIZE_PARAM_IS_A_RESERVATION = 0x00010000;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
HANDLE CreateThread(
  LPSECURITY_ATTRIBUTES threadAttributes,
  size_t stackSize,
  LPTHREAD_START_ROUTINE startAddress,
  void* parameter,
  DWORD creationFlags,
  LPDWORD outThreadId
) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
HRESULT SetThreadDescription(HANDLE thread, const wchar_t* description) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreaddescription
HRESULT GetThreadDescription(HANDLE thread, wchar_t** outDescription) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace Win32
