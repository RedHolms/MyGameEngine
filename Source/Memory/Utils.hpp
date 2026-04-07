#pragma once

extern void __implementation_memcpy(void*, void const*, size_t);
extern void __implementation_memmove(void*, void const*, size_t);

namespace Memory {

inline void Copy(void* dst, void const* src, size_t bytesCount) {
  __implementation_memcpy(dst, src, bytesCount);
}

inline void Move(void* dst, void const* src, size_t bytesCount) {
  __implementation_memmove(dst, src, bytesCount);
}

} // namespace Memory
