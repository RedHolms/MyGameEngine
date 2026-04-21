#pragma once

#include "Win32/Base.hpp"

_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  DWORD, GetCurrentProcessId, void)
_DECLARE_WIN32_NORETURN_FUNCTION(Kernel32.dll, Kernel32.lib,
  void, ExitProcess, UINT)

namespace Win32 {

enum class ProcessID : DWORD {};

forceinline ProcessID GetCurrentProcessID() noexcept {
  return static_cast<ProcessID>(_CALL_WIN32_FUNCTION(GetCurrentProcessId));
}

[[noreturn]] forceinline void Exit(uint exitCode) noexcept {
  _CALL_WIN32_FUNCTION(ExitProcess, exitCode);
}

} // namespace Win32
