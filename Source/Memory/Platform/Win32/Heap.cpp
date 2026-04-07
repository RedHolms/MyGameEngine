#include "Heap.hpp"
#include "Memory/Heap.hpp"

#include "Win32/Heap.hpp"

static Win32::RAW_HANDLE __global_heap = Win32::NULL_HANDLE;

void __init_heap_allocators() {
  __global_heap = Win32::Heap::GetProcessHeap();
}

void* Memory::Allocate(size_t bytesCount) {
  return Win32::Heap::Alloc(__global_heap, 0, bytesCount);
}

void Memory::Deallocate(void* block) {
  Win32::Heap::Free(__global_heap, 0, block);
}
