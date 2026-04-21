#pragma once

#include "Win32/Base.hpp"

_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  DWORD, GetCurrentThreadId, void)
_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  BOOL, SwitchToThread, void)

namespace Win32 {

enum class ThreadID : DWORD {};

forceinline ThreadID GetCurrentThreadID() noexcept {
  return static_cast<ThreadID>(_CALL_WIN32_FUNCTION(GetCurrentThreadId));
}

forceinline bool Yield() noexcept {
  return _CALL_WIN32_FUNCTION(SwitchToThread) != 0;
}

} // namespace Win32
