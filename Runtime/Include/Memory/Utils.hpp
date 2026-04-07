#pragma once

extern "C" {

void __Impl_MemoryCopy(void*, void const*, size_t);
void __Impl_MemoryMove(void*, void const*, size_t);

} // extern "C"

namespace Memory {

forceinline void CopyBytes(void* dst, void const* src, size_t bytesCount) {
  __Impl_MemoryCopy(dst, src, bytesCount);
}

template <typename T>
constexpr void CopyItems(ExplicitType<T>* dst, ExplicitType<T> const* src, size_t itemsCount) {
  static_assert(IsTriviallyCopyableType<T>, "Memory::CopyItems<> requires trivially copyable type");

  if (!IsConstantEvaluated()) {
    CopyBytes(dst, src, itemsCount * sizeof(T));
    return;
  }

  for (size_t i = 0; i < itemsCount; ++i)
    dst[i] = src[i];
}

forceinline void MoveBytes(void* dst, void const* src, size_t bytesCount) {
  __Impl_MemoryMove(dst, src, bytesCount);
}

template <typename T>
constexpr void MoveItems(ExplicitType<T>* dst, ExplicitType<T> const* src, size_t itemsCount) {
  static_assert(IsTriviallyCopyableType<T>, "Memory::MoveItems<> requires trivially copyable type");

  if (!IsConstantEvaluated()) {
    MoveBytes(dst, src, itemsCount * sizeof(T));
    return;
  }

  if (dst < src) {
    for (size_t i = 0; i < itemsCount; ++i)
      dst[i] = src[i];
  }
  else {
    for (ssize_t i = itemsCount; i-- > 0;)
      dst[i] = src[i];
  }
}

} // namespace Memory