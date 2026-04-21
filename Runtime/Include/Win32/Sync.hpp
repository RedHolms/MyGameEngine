#pragma once

#include "Win32/Base.hpp"

_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  void, Sleep, DWORD)

namespace Win32 {

forceinline void StupidSleep(uint milliseconds) noexcept {
  _CALL_WIN32_FUNCTION(Sleep, milliseconds);
}

} // namespace Win32
