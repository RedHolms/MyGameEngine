#include "Memory/Allocators.hpp"

#include "Heap.hpp"
#include "Win32Api/Heap.hpp"

void* Memory::Allocate(size_t bytesCount) {
  return Win32::Heap::Alloc(__mgert_global_heap, 0, bytesCount);
}

void Memory::Deallocate(void* block) {
  Win32::Heap::Free(__mgert_global_heap, 0, block);
}
