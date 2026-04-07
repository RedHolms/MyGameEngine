#include "Win32Api/SystemTime.hpp"

#include "DeclSpecs.hpp"

extern "C" {

WIN32_FUNC void WIN32_API GetSystemTimeAsFileTime(Win32::FileTime*);
WIN32_FUNC uint32_t WIN32_API GetTickCount();
WIN32_FUNC uint64_t WIN32_API GetTickCount64();

} // extern "C"

Win32::FileTime Win32::GetSystemTimeAsFileTime() {
  FileTime result;
  ::GetSystemTimeAsFileTime(&result);
  return result;
}

uint32_t Win32::GetSystemRunTimeInMs32() {
  return ::GetTickCount();
}

uint64_t Win32::GetSystemRunTimeInMs() {
  return ::GetTickCount64();
}
