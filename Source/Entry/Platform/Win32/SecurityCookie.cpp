#include "SecurityCookie.hpp"

extern "C" platform_word_t __security_cookie = 0;
extern "C" platform_word_t __security_cookie_complement = 0;

// __declspec(safebuffers) - disable cookie check for this call as it's not yet\
//   initialized.
extern "C" void __declspec(safebuffers) __security_init_cookie() {
  if (__security_cookie != 0)
    return;

  LARGE_INTEGER perf;
  QueryPerformanceCounter(&perf);

  platform_word_t cookie = 0;

  cookie ^= (platform_word_t)&cookie;
  cookie ^= (platform_word_t)GetCurrentProcessId();
  cookie ^= (platform_word_t)GetCurrentThreadId();
  cookie ^= (platform_word_t)GetTickCount64();
  cookie ^= (platform_word_t)perf.QuadPart;

  // Avoid bad values
  if (cookie == 0 || cookie == 0xBB40E64E)
    cookie += 0x10000;

  __security_cookie = cookie;
  __security_cookie_complement = ~cookie;
}

extern "C" void __security_check_cookie(platform_word_t cookie) {
  if (cookie != __security_cookie) {
    __debugbreak();
  }
}
