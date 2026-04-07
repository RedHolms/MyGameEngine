#include "GlobalsInit.hpp"
#include "Memory/Platform/Win32/Heap.hpp"
#include "SecurityCookie.hpp"
#include "Win32/Process.hpp"

// Engine's main procedure
extern uint EngineMain();

/**
 * Program entry point.
 * It's purpose to initialize everything low-level and platform-dependent.
 * Engine/game startup is located inside the "EngineMain()" procedure.
 */
extern "C" void __MyGameEngine_EntryPoint() {
  // Must be the first thing to be done
  __security_init_cookie();

  __init_heap_allocators();
  __init_global_terms();

  // Execute engine's main procedure
  uint exitCode = EngineMain();

  // Terminate...

  Win32::Process::Exit(exitCode);
}
