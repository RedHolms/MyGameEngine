#include "Win32Api/CurrentProcess.hpp"

#include "DeclSpecs.hpp"

extern "C" {

[[noreturn]] WIN32_FUNC void WIN32_API ExitProcess(uint);
WIN32_FUNC uint32_t WIN32_API GetCurrentProcessId();

} // extern "C"

void Win32::CurrentProcess::Exit(uint exitCode) {
  ::ExitProcess(exitCode);
}

Win32::ProcessID Win32::CurrentProcess::GetID() {
  return ::GetCurrentProcessId();
}
