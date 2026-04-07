#pragma once

#include "Allocators.hpp"

namespace Memory {

_ALLOCATOR_SPECS void* Allocate(size_t bytesCount);
void Deallocate(void* block);

} // namespace Memory
