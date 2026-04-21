#include "Cookie.hpp"

#include <Intrinsics/Interrupts.hpp>
#include <Win32/Process.hpp>
#include <Win32/Thread.hpp>
#include <Win32/Time.hpp>

constexpr uintptr_t DEFAULT_SECURITY_COOKIE = 0x00002B992DDFA232;

extern "C" {

// Compiler looks for these names:
uintptr_t __security_cookie = DEFAULT_SECURITY_COOKIE;
void __security_check_cookie(uintptr_t);

} // extern "C"

void __declspec(safebuffers) SecurityCookie::Initialize() {
  if (__security_cookie != DEFAULT_SECURITY_COOKIE)
    return;

  {
    Win32::FILETIME systemTime;
    Win32::GetSystemTimeAsFileTime(&systemTime);
    __security_cookie = systemTime.ToLargeInteger();
  }

  __security_cookie ^= static_cast<uint32_t>(Win32::GetCurrentThreadID());
  __security_cookie ^= static_cast<uint32_t>(Win32::GetCurrentProcessID());

  __security_cookie ^= Win32::GetSystemRunTimeInMs() << 56;
  __security_cookie ^= Win32::GetSystemRunTimeInMs();

  __security_cookie ^= reinterpret_cast<uintptr_t>(&__security_cookie);

  __security_cookie &= 0x0000FFFFFFFFFFFFi64;

  if (__security_cookie == DEFAULT_SECURITY_COOKIE)
    ++__security_cookie;
}

void __security_check_cookie(uintptr_t cookie) {
  if (cookie == __security_cookie && (__security_cookie & 0xFFFF000000000000i64) == 0)
    return;

  // Security failure!
  Intrinsics::FailFast(-1);
}
