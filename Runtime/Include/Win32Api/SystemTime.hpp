#pragma once

#include "Base.hpp"
#include "FileTime.hpp"

namespace Win32 {

// Win32 name: GetSystemTimeAsFileTime
FileTime GetSystemTimeAsFileTime();

// Win32 name: GetTickCount
uint32_t GetSystemRunTimeInMs32();

// Win32 name: GetTickCount64
uint64_t GetSystemRunTimeInMs();

} // namespace Win32
