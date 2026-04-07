#include "Win32Api/CurrentThread.hpp"

#include "DeclSpecs.hpp"

extern "C" {

WIN32_FUNC uint32_t WIN32_API GetCurrentThreadId();

} // extern "C"

Win32::ThreadID Win32::CurrentThread::GetID() {
  return ::GetCurrentThreadId();
}
