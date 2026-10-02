#pragma once

#include <MyGameEngine/IntTypes.hpp>
#include <cstring>
#include <type_traits>

namespace MemoryUtils {

inline void CopyBytes(void* dst, void const* src, size_t bytesCount) {
  memcpy(dst, src, bytesCount);
}

inline void FillBytes(void* dst, uint8 value, size_t bytesCount) {
  memset(dst, value, bytesCount);
}

template <typename T>
constexpr void Copy(std::type_identity_t<T>* dst, std::type_identity_t<T> const* src, size_t itemsCount) {
  if not consteval {
    CopyBytes(dst, src, itemsCount * sizeof(T));
  }
  else {
    for (index_t i = 0; i < itemsCount; ++i)
      dst[i] = src[i];
  }
}

} // namespace MemoryUtils
