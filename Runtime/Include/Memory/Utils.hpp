#pragma once

namespace Memory {
namespace Detail {

// Defined in assembly:
extern "C" void* memset(void*, uint8_t, size_t);

// Our 'memcpy' basically is a 'memmove'. We keep 'memmove' as an alias in case compiler calls it.
extern "C" void* memcpy(void*, void const*, size_t);
extern "C" void* memmove(void*, void const*, size_t);

} // namespace Details

forceinline void CopyBytes(void* dst, void const* src, size_t bytesCount) {
  Detail::memcpy(dst, src, bytesCount);
}

forceinline void FillBytes(void* dst, uint8_t value, size_t bytesCount) {
  Detail::memset(dst, value, bytesCount);
}

template <typename T>
constexpr void Copy(ExplicitType<T>* dst, ExplicitType<T> const* src, size_t itemsCount) {
  if (!IsConstantEvaluated()) {
    CopyBytes(dst, src, itemsCount * sizeof(T));
  }
  else {
    if (dst < src) {
      for (size_t i = 0; i < itemsCount; ++i)
        dst[i] = src[i];
    }
    else {
      for (ssize_t i = itemsCount; i-- > 0;)
        dst[i] = src[i];
    }
  }
}

} // namespace Memory
