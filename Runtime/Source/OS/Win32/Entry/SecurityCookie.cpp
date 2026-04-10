#include "SecurityCookie.hpp"

#include "Win32Api/HighResolutionTimer.hpp"
#include "Win32Api/Processes.hpp"
#include "Win32Api/SystemTime.hpp"
#include "Win32Api/Threads.hpp"

#ifdef IS_64_BIT
#define DEFAULT_SECURITY_COOKIE 0x00002B992DDFA232
#else
#define DEFAULT_SECURITY_COOKIE 0xBB40E64E
#endif

extern "C" {

machine_word_t __security_cookie = DEFAULT_SECURITY_COOKIE;
machine_word_t __security_cookie_complement = ~DEFAULT_SECURITY_COOKIE;

void __security_check_cookie(machine_word_t cookie);

} // extern "C"

// Stollen from Microsoft's CRT
static machine_word_t GenerateSecurityCookie() {
  machine_word_t cookie;

  Win32::FileTime systemTime = Win32::GetSystemTimeAsFileTime();
#ifdef IS_64_BIT
  cookie = systemTime.fullDateTime;
#else
  cookie = systemTime.lowDateTime;
  cookie ^= systemTime.highDateTime;
#endif

  cookie ^= Win32::CurrentThread::GetID();
  cookie ^= Win32::CurrentProcess::GetID();

#ifdef IS_64_BIT
  cookie ^= Win32::GetSystemRunTimeInMs() << 56;
#endif
  cookie ^= static_cast<machine_word_t>(Win32::GetSystemRunTimeInMs());

  uint64_t hrtCount = Win32::HighResolutionTimer::GetCount();
#ifdef IS_64_BIT
  cookie ^= ((hrtCount & 0xFFFFFFFF) << 32) ^ hrtCount;
#else
  cookie ^= hrtCount & 0xFFFFFFFF;
  cookie ^= (hrtCount >> 32) & 0xFFFFFFFF;
#endif

  cookie ^= reinterpret_cast<uintptr_t>(&cookie);

#if IS_64_BIT
  /*
   * On Win64, generate a cookie with the most significant word set to zero,
   * as a defense against buffer overruns involving null-terminated strings.
   * Don't do so on Win32, as it's more important to keep 32 bits of cookie.
   */
  cookie &= 0x0000FFFFFFFFFFFFi64;
#endif /* defined (_WIN64) */

  return cookie;
}

void __declspec(safebuffers) __mgert_init_security_cookie() {
  if (
    __security_cookie != DEFAULT_SECURITY_COOKIE
#if IS_32_BIT
    && (__security_cookie & 0xFFFF0000) != 0
#endif
  ) {
    __security_cookie_complement = ~__security_cookie;
    return;
  }

  machine_word_t cookie = GenerateSecurityCookie();

  if (cookie == DEFAULT_SECURITY_COOKIE) {
    ++cookie;
  }
#if IS_32_BIT
  else if ((cookie & 0xFFFF0000) == 0) {
    cookie |= ((cookie | 0x4711) << 16);
  }
#endif

  __security_cookie = cookie;
  __security_cookie_complement = ~cookie;
}

void __declspec(noreturn) __fastfail(uint);
#pragma intrinsic(__fastfail)

void __security_check_cookie(machine_word_t cookie) {
  if (cookie != __security_cookie) {
    __fastfail(2);
  }
}
