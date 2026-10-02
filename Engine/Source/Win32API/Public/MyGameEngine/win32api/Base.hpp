#pragma once

#include <MyGameEngine/Platform/PlatformInfo.hpp>

#define _WIN32_FUNC_ATTRS __attribute__((stdcall)) __attribute__((dllimport))

namespace win32api {

typedef void *PVOID, *LPVOID;
typedef int BOOL;
typedef unsigned char BOOLEAN;

typedef unsigned long DWORD, *PDWORD, *LPDWORD;

typedef unsigned int UINT, *PUINT, *LPUINT;

#ifndef IS_64BIT
#error "todo"
#endif

typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;

typedef __int64 INT_PTR, *PINT_PTR, *LPINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR, *LPUINT_PTR;

typedef __int64 LONG_PTR, *PLONG_PTR, *LPLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR, *LPULONG_PTR;

// Any handle type
typedef void* HANDLE;

typedef long HRESULT;

constexpr auto nullhandle = nullptr;

// Handle types
#define _DECLARE_HANDLE(NAME) typedef struct NAME##__ { int __Unused; } *NAME
_DECLARE_HANDLE(HWND);
_DECLARE_HANDLE(HINSTANCE);
using HMODULE = HINSTANCE;
_DECLARE_HANDLE(HICON);
_DECLARE_HANDLE(HCURSOR);
_DECLARE_HANDLE(HBRUSH);
_DECLARE_HANDLE(HMENU);
#undef _DECLARE_HANDLE

typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;

template <typename T>
constexpr bool SUCCEEDED(T value) noexcept {
  return static_cast<HRESULT>(value) >= 0;
}

template <typename T>
constexpr bool FAILED(T value) noexcept {
  return static_cast<HRESULT>(value) < 0;
}

// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-localfree
extern "C" HLOCAL LocalFree(HLOCAL mem) _WIN32_FUNC_ATTRS;

typedef struct SECURITY_ATTRIBUTES {
  DWORD   nLength; // = sizeof(SECURITY_ATTRIBUTES)
  LPVOID  lpSecurityDescriptor;
  BOOL    bInheritHandle;
} *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

struct POINT {
  long x;
  long y;
};

// Used for creating threads and processes
constexpr DWORD CREATE_SUSPENDED = 0x00000004;

} // namespace win32api
