#include "Intrinsics/Interrupts.hpp"

// Called by the 'throw' statement
extern "C" void __declspec(noreturn) __stdcall _CxxThrowException(void* object, void* throwInfo) noexcept(false) {
  (void)object;
  (void)throwInfo;
  Intrinsics::DebugBreak();
}
