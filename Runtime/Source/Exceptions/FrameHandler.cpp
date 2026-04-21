#include "Intrinsics/Interrupts.hpp"

struct EHRegistrationNode {
  /* void *			stackPtr */		// Stack ptr at entry to try (below address point)
  EHRegistrationNode*	pNext;			// Next node in the chain
  void * 				frameHandler;	// The handler function for this frame
  int			state;			// The current state of this function
};

extern "C" int __cdecl __CxxFrameHandler4(
void  *pExcept,         // Information for this exception
EHRegistrationNode RN,               // Dynamic information for this frame
void            *pContext,        // Context info
void  *pDC              // More dynamic info for this frame
) {
  Intrinsics::DebugBreak();
  return 0;
}
