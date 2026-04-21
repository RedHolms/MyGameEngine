#pragma once

#include "Base.hpp"

namespace Win32 {

struct FILETIME {
  DWORD lowDateTime;
  DWORD highDateTime;

  constexpr uint64_t ToLargeInteger() const noexcept {
    ULARGE_INTEGER result;
    result.lowPart = lowDateTime;
    result.highPart = highDateTime;
    return result.quadPart;
  }
};

} // namespace Win32

_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  void, GetSystemTimeAsFileTime, Win32::FILETIME*)
_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  uint32_t, GetTickCount, void)
_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  uint64_t, GetTickCount64, void)

namespace Win32 {

forceinline void GetSystemTimeAsFileTime(FILETIME* outFileTime) noexcept {
  _CALL_WIN32_FUNCTION(GetSystemTimeAsFileTime, outFileTime);
}

forceinline uint32_t GetSystemRunTimeInMs32() noexcept {
  return _CALL_WIN32_FUNCTION(GetTickCount);
}

forceinline uint64_t GetSystemRunTimeInMs() noexcept {
  return _CALL_WIN32_FUNCTION(GetTickCount64);
}

} // namespace Win32
