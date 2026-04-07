#pragma once

#include "Memory/Allocators.hpp"
#include "Base.hpp"

namespace Win32::Heap {

// Win32 name: GetProcessHeap
RAW_HANDLE GetProcessHeap();

// Win32 name: HeapAlloc
_ALLOCATOR_SPECS void* Alloc(RAW_HANDLE heap, uint32_t flags, size_t bytesCount);

// Win32 name: HeapFree
bool Free(RAW_HANDLE heap, uint32_t flags, void* block);

} // namespace Win32::Heap
