#pragma once

extern "C" {

extern machine_word_t __security_cookie;
extern void __security_init_cookie();
extern void __security_check_cookie(machine_word_t cookie);

} // extern "C"
