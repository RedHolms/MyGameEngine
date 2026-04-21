#include "Security/Cookie.hpp"
#include <Win32/Process.hpp>

// User entry point
extern uint Main();

using init_fn_t = void (*)();
using init_fn_e_t = int (*)();

#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)
#pragma section(".CRT$XPA", long, read)
#pragma section(".CRT$XPZ", long, read)
#pragma section(".CRT$XTA", long, read)
#pragma section(".CRT$XTZ", long, read)

extern "C" __declspec(allocate(".CRT$XIA")) init_fn_e_t __xi_a[] = { nullptr }; // C initializers (first)
extern "C" __declspec(allocate(".CRT$XIZ")) init_fn_e_t __xi_z[] = { nullptr }; // C initializers (last)
extern "C" __declspec(allocate(".CRT$XCA")) init_fn_t __xc_a[] = { nullptr };   // C++ initializers (first)
extern "C" __declspec(allocate(".CRT$XCZ")) init_fn_t __xc_z[] = { nullptr };   // C++ initializers (last)
extern "C" __declspec(allocate(".CRT$XPA")) init_fn_t __xp_a[] = { nullptr };   // C pre-terminators (first)
extern "C" __declspec(allocate(".CRT$XPZ")) init_fn_t __xp_z[] = { nullptr };   // C pre-terminators (last)
extern "C" __declspec(allocate(".CRT$XTA")) init_fn_t __xt_a[] = { nullptr };   // C terminators (first)
extern "C" __declspec(allocate(".CRT$XTZ")) init_fn_t __xt_z[] = { nullptr };   // C terminators (last)

#pragma comment(linker, "/merge:.CRT=.rdata")

static void _initterm(init_fn_t* first, init_fn_t* last) {
  for (init_fn_t* it = first; it != last; ++it) {
    if (*it == nullptr)
      continue;

    (**it)();
  }
}

static int _initterm_e(init_fn_e_t* first, init_fn_e_t* last) {
  for (init_fn_e_t* it = first; it != last; ++it) {
    if (*it == nullptr)
      continue;

    int const result = (**it)();
    if (result != 0)
      return result;
  }

  return 0;
}

static bool _run_global_initializers() {
  if (_initterm_e(__xi_a, __xi_z) != 0)
    return false;

  _initterm(__xc_a, __xc_z);
  return true;
}

static void _run_global_terminators() {
  _initterm(__xp_a, __xp_z);
  _initterm(__xt_a, __xt_z);
}

// Bare program entry point
extern "C" void __MyGameEngineRuntime_EntryPoint(void) {
  SecurityCookie::Initialize();
  Memory::Detail::gAppHeap = Win32::GetProcessHeap();

  if (_run_global_initializers() == false) {
    // Failed
    Win32::Exit(1);
  }

  auto const exitCode = Main();
  _run_global_terminators();

  Win32::Exit(exitCode);
}
