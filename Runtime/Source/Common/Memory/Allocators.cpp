#include "Memory/Allocators.hpp"

_ALLOCATOR_SPECS void* operator new(size_t size) {
  return Memory::Allocate(size);
}

_ALLOCATOR_SPECS void* operator new[](size_t size) {
  return Memory::Allocate(size);
}

void operator delete(void* block) {
  Memory::Deallocate(block);
}

void operator delete(void* block, size_t) {
  Memory::Deallocate(block);
}

void operator delete[](void* block) {
  Memory::Deallocate(block);
}

void operator delete[](void* block, size_t) {
  Memory::Deallocate(block);
}
