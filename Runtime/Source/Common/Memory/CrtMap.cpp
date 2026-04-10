#include "Memory/Utils.hpp"

// Map crt memory functions as they're can be used by compiler implicitly

extern "C" void memset(void*, uint8_t, size_t);

void memset(void* dst, uint8_t value, size_t bytesCount) {
  Memory::FillBytes(dst, value, bytesCount);
}
