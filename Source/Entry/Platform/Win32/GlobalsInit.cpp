#include "GlobalsInit.hpp"

using init_fn = void(__cdecl*)();

#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

__declspec(allocate(".CRT$XIA")) init_fn __xi_a[] = { nullptr };
__declspec(allocate(".CRT$XIZ")) init_fn __xi_z[] = { nullptr };

__declspec(allocate(".CRT$XCA")) init_fn __xc_a[] = { nullptr };
__declspec(allocate(".CRT$XCZ")) init_fn __xc_z[] = { nullptr };

static void initterm(init_fn* begin, init_fn* end) {
  while (begin < end) {
    if (*begin)
      (*begin)();
    ++begin;
  }
}

void __init_global_terms() {
  initterm(__xi_a, __xi_z);
  initterm(__xc_a, __xc_z);
}
