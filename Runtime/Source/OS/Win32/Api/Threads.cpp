#include "Win32Api/CurrentThread.hpp"

#include "DeclSpecs.hpp"

extern "C" {

WIN32_FUNC uint32_t WIN32_API GetCurrentThreadId();
WIN32_FUNC uint32_t WIN32_API SwitchToThread();
WIN32_FUNC void WIN32_API Sleep(uint32_t);

} // extern "C"

Win32::ThreadID Win32::CurrentThread::GetID() {
  return ::GetCurrentThreadId();
}

bool Win32::CurrentThread::Yield() {
  return !!::SwitchToThread();
}

void Win32::CurrentThread::StupidSleep(uint milliseconds) {
  ::Sleep(milliseconds);
}
