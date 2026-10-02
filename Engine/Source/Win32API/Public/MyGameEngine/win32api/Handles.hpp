#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
BOOL CloseHandle(HANDLE handle) _WIN32_FUNC_ATTRS;

constexpr DWORD DUPLICATE_CLOSE_SOURCE = 0x00000001;
constexpr DWORD DUPLICATE_SAME_ACCESS = 0x00000002;

// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-duplicatehandle
BOOL DuplicateHandle(
  HANDLE sourceProcess, HANDLE source,
  HANDLE targetProcess, HANDLE* outTarget,
  DWORD desiredAccess, BOOL inheritHandle,
  DWORD options
);

} // extern "C"
} // namespace Win32
