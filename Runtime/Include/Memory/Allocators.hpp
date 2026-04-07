#pragma once

#define _ALLOCATOR_SPECS __declspec(allocator) __declspec(restrict)

namespace Memory {

_ALLOCATOR_SPECS void* Allocate(size_t bytesCount);
void Deallocate(void* block);

} // namespace Memory

[[nodiscard]] [[msvc::constexpr]] inline void* operator new(size_t size, void* where) noexcept {
  (void)size;
  return where;
}

inline void operator delete(void*, void*) noexcept {}

[[nodiscard]] inline void* operator new[](size_t size, void* where) noexcept {
  (void)size;
  return where;
}

inline void operator delete[](void*, void*) noexcept {}
