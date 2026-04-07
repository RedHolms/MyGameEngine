#include "Process.hpp"

extern "C" {

[[noreturn]] WIN32_FUNC void WIN32_API ExitProcess(uint);

} // extern "C"

void Win32::Process::Exit(uint exitCode) {
  ::ExitProcess(exitCode);
}
