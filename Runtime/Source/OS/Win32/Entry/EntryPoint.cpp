#include "ISA.hpp"
#include "Initializers.hpp"
#include "OS/Win32/Memory/Heap.hpp"
#include "SecurityCookie.hpp"
#include "Win32Api/CurrentProcess.hpp"

// Real entry point
extern uint Main();

/**
 * Program entry point.
 * It's purpose to initialize everything low-level and platform-dependent.
 */
extern "C" void __MyGameEngineRuntime_EntryPoint() {
  __mgert_init_security_cookie();
  __mgert_init_global_heap();
  __mgert_isa_init();
  __mgert_run_initializers();

  uint exitCode = Main();

  __mgert_run_terminators();

  Win32::CurrentProcess::Exit(exitCode);
}
