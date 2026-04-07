#include "Win32Api/HighResolutionTimer.hpp"

#include "DeclSpecs.hpp"

extern "C" {

WIN32_FUNC uint32_t WIN32_API QueryPerformanceCounter(uint64_t*);
WIN32_FUNC uint32_t WIN32_API QueryPerformanceFrequency(uint64_t*);

} // extern "C"

uint64_t Win32::HighResolutionTimer::GetCount() {
  uint64_t result;
  ::QueryPerformanceCounter(&result);
  return result;
}

uint64_t Win32::HighResolutionTimer::GetFrequency() {
  uint64_t result;
  ::QueryPerformanceFrequency(&result);
  return result;
}
