#include "Intrinsics/Interrupts.hpp"

// Used by the compiler as placeholder for pure (virtual with '= 0') member functions
extern "C" int __cdecl _purecall() {
  Intrinsics::DebugBreak();
  return 0;
}
