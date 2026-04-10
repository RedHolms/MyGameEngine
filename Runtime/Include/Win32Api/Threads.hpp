#pragma once

namespace Win32 {

using ThreadID = uint32_t;

namespace CurrentThread {

// Win32 name: GetCurrentThreadId
ThreadID GetID();

// Win32 name: SwitchToThread
bool Yield();

// Win32 name: Sleep
void StupidSleep(uint milliseconds);

} // namespace CurrentThread

} // namespace Win32
