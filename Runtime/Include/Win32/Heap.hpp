#pragma once

#include "Win32/Base.hpp"

_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  HANDLE, GetProcessHeap, void)
_DECLARE_WIN32_ALLOCATOR_FUNCTION(Kernel32.dll, Kernel32.lib,
  void*, HeapAlloc, HANDLE, DWORD, size_t)
_DECLARE_WIN32_FUNCTION(Kernel32.dll, Kernel32.lib,
  BOOL, HeapFree, HANDLE, DWORD, void*)

namespace Win32 {

forceinline HANDLE GetProcessHeap() noexcept {
  return _CALL_WIN32_FUNCTION(GetProcessHeap);
}

namespace Heap {

enum class AllocFlags : DWORD {
  NONE = 0,
  NO_SERIALIZE = 0x00000001,
  GENERATE_EXCEPTIONS = 0x00000004,
  ZERO_MEMORY = 0x00000008
};

forceinline __declspec(allocator) void* Alloc(HANDLE heapHandle, AllocFlags flags, size_t bytesCount) noexcept {
  return _CALL_WIN32_FUNCTION(HeapAlloc, heapHandle, static_cast<DWORD>(flags), bytesCount);
}

enum class FreeFlags : DWORD {
  NONE = 0,
  NO_SERIALIZE = 0x00000001
};

forceinline bool Free(HANDLE heapHandle, FreeFlags flags, void* block) noexcept {
  return _CALL_WIN32_FUNCTION(HeapFree, heapHandle, static_cast<DWORD>(flags), block);
}

} // namespace Heap

} // namespace Win32
