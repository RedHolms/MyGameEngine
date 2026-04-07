#pragma once

#include "Base.hpp"
#include "Memory/Allocators.hpp"

namespace Win32::Heap {

RAW_HANDLE GetProcessHeap();
_ALLOCATOR_SPECS void* Alloc(RAW_HANDLE heap, uint32_t flags, size_t bytesCount);
bool Free(RAW_HANDLE heap, uint32_t flags, void* block);

} // namespace Win32::Heap
