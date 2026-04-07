#pragma once

namespace Win32::HighResolutionTimer {

// Win32 name: QueryPerformanceCounter
uint64_t GetCount();

// Win32 name: QueryPerformanceFrequency
uint64_t GetFrequency();

} // namespace Win32::HighResolutionTimer

