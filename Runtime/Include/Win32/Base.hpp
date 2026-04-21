#pragma once

#define __DECLARE_WIN32_FUNCTION(RET_TYPE, ATTRS, SPECS, NAME, ...)                                                    \
  namespace Win32::Detail { extern "C" ATTRS __declspec(dllimport) SPECS RET_TYPE __stdcall NAME(__VA_ARGS__); }

#define _DECLARE_WIN32_FUNCTION(DLL_NAME, LIB_NAME, RET_TYPE, NAME, ...)                                               \
  __DECLARE_WIN32_FUNCTION(RET_TYPE, /**/, /**/, NAME, __VA_ARGS__)

#define _DECLARE_WIN32_NORETURN_FUNCTION(DLL_NAME, LIB_NAME, RET_TYPE, NAME, ...)                                      \
  __DECLARE_WIN32_FUNCTION(RET_TYPE, [[noreturn]], /**/, NAME, __VA_ARGS__)

#define _DECLARE_WIN32_ALLOCATOR_FUNCTION(DLL_NAME, LIB_NAME, RET_TYPE, NAME, ...)                                     \
  __DECLARE_WIN32_FUNCTION(RET_TYPE, /**/, __declspec(allocator), NAME, __VA_ARGS__)

#define _CALL_WIN32_FUNCTION(NAME, ...)                                                                                \
  ::Win32::Detail::NAME(__VA_ARGS__)

namespace Win32 {

using INT_PTR = intptr_t;
using UINT_PTR = uintptr_t;
using LONG_PTR = intptr_t;
using ULONG_PTR = uintptr_t;

using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned long;

using FLOAT = float;

using BOOL = int;

using CHAR = char;
using UCHAR = unsigned char;
using SHORT = short;
using USHORT = unsigned short;
using INT = int;
using UINT = unsigned int;
using LONG = long;
using ULONG = unsigned long;

using WPARAM = UINT_PTR;
using LPARAM = LONG_PTR;
using LRESULT = LONG_PTR;

using HANDLE = void*;
#define _WIN32_DECLARE_HANDLE(NAME) struct NAME##__ { int __Unused; }; using NAME = NAME##__*;
#define NULL_HANDLE nullptr

using HRESULT = long;

using LONGLONG = long long;
using ULONGLONG = unsigned long long;

union LARGE_INTEGER {
  struct {
    DWORD lowPart;
    LONG highPart;
  };
  LONGLONG quadPart;
};

union ULARGE_INTEGER {
  struct {
    DWORD lowPart;
    DWORD highPart;
  };
  ULONGLONG quadPart;
};

} // namespace Win32
