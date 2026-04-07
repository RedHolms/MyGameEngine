#include "SecurityCookie.hpp"

extern "C" machine_word_t __security_cookie = 0;
extern "C" machine_word_t __security_cookie_complement = 0;

// __declspec(safebuffers) - disable cookie check for this call as it's not yet\
//   initialized.
extern "C" void __declspec(safebuffers) __security_init_cookie() {
  if (__security_cookie != 0)
    return;

  // FIXME this is shit is a shame
  __security_cookie = 0x0123456789ABCDEF;
  __security_cookie_complement = ~__security_cookie;
}

extern "C" void __security_check_cookie(machine_word_t cookie) {
  if (cookie != __security_cookie) {
    __debugbreak();
  }
}
