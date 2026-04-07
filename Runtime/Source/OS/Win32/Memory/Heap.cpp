#include "Heap.hpp"

#include "Win32Api/Heap.hpp"

Win32::RAW_HANDLE __mgert_global_heap = Win32::NULL_HANDLE;

void __mgert_init_global_heap() {
  __mgert_global_heap = Win32::Heap::GetProcessHeap();
}
