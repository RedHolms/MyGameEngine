#pragma once

#include "Win32/Heap.hpp"

class AllocationFailedException final : public RuntimeException {
public:
  explicit inline AllocationFailedException(size_t bytesCountTriedToAllocate) noexcept
    : RuntimeException(U"Memory allocation failed"s),
      m_bytesCount(bytesCountTriedToAllocate)
  {}

public:
  inline TypeInfo const& GetExceptionType() const noexcept override {
    return typeid(AllocationFailedException);
  }

  inline size_t GetBytesCount() const noexcept {
    return m_bytesCount;
  }

private:
  size_t m_bytesCount;
};

namespace Memory {

namespace Detail {

extern Win32::HANDLE gAppHeap;

} // namespace Detail

forceinline __declspec(allocator) void* Allocate(size_t bytesCount) noexcept {
  return Win32::Heap::Alloc(Detail::gAppHeap, Win32::Heap::AllocFlags::NONE, bytesCount);
}

forceinline void Deallocate(void* block) noexcept {
  Win32::Heap::Free(Detail::gAppHeap, Win32::Heap::FreeFlags::NONE, block);
}

} // namespace Memory

inline __declspec(allocator) void* operator new(size_t size) {
  auto const block = Memory::Allocate(size);

  if (block == nullptr)
    throw AllocationFailedException(size);

  return block;
}

inline __declspec(allocator) void* operator new[](size_t size) {
  return operator new(size);
}

inline void operator delete(void* block) noexcept {
  if (block != nullptr)
    Memory::Deallocate(block);
}

inline void operator delete(void* block, size_t) noexcept {
  operator delete(block);
}

inline void operator delete[](void* block) noexcept {
  operator delete(block);
}

inline void operator delete[](void* block, size_t) noexcept {
  operator delete(block);
}

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
