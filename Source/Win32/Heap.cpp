#include "Heap.hpp"

using Win32::RAW_HANDLE;

extern "C" {

WIN32_FUNC RAW_HANDLE WIN32_API GetProcessHeap();
WIN32_FUNC _ALLOCATOR_SPECS void* WIN32_API HeapAlloc(RAW_HANDLE, uint32_t, size_t);
WIN32_FUNC uint32_t WIN32_API HeapFree(RAW_HANDLE, uint32_t, void*);

} // extern "C"

RAW_HANDLE Win32::Heap::GetProcessHeap() {
  return ::GetProcessHeap();
}

void* Win32::Heap::Alloc(RAW_HANDLE heap, uint32_t flags, size_t bytesCount) {
  return ::HeapAlloc(heap, flags, bytesCount);
}

bool Win32::Heap::Free(RAW_HANDLE heap, uint32_t flags, void* block) {
  return ::HeapFree(heap, flags, block) != 0;
}
