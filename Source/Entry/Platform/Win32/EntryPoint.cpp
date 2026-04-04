#include "GlobalsInit.hpp"
#include "SecurityCookie.hpp"

// Engine's main procedure
extern uint EngineMain();

/**
 * Program entry point.
 */
extern "C" void __MyGameEngine_EntryPoint() {
  // Must be the first thing to be done
  __security_init_cookie();

  // Run static initializers
  __init_global_terms();

  // Execute engine's main procedure
  uint exitCode = EngineMain();

  // Terminate...

  ExitProcess(exitCode);
}
