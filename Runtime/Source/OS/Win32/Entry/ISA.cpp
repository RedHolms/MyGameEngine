#include "ISA.hpp"

extern "C" {

int __isa_available = 0;
int __isa_enabled = 0;
int __favor = 0;
machine_word_t __memset_fast_string_threshold = 0x80000;
machine_word_t __memset_nt_threshold = 0x2000000;

}

void __mgert_isa_init() {
  // TODO
}
