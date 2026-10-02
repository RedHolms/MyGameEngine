#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

// https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-getprocessheap
HANDLE GetProcessHeap(void) _WIN32_FUNC_ATTRS;

constexpr DWORD HEAP_NO_SERIALIZE = 0x00000001;
constexpr DWORD HEAP_GENERATE_EXCEPTIONS = 0x00000004;
constexpr DWORD HEAP_ZERO_MEMORY = 0x00000008;

// https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapalloc
void* HeapAlloc(HANDLE heap, DWORD flags, size_t bytes) _WIN32_FUNC_ATTRS;

// https://learn.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapfree
BOOL HeapFree(HANDLE heap, DWORD flags, void* block) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace Win32
