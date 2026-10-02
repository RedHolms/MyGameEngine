#pragma once

#define MGE_ASSERT(EXPR, MSG) ::MyGameEngine::Assert((EXPR), #EXPR, __FILE__, __LINE__, MSG)

namespace MyGameEngine {

namespace Internal {

void HandleAssertationFailure(const char* expression, const char* sourceFile, int sourceLine, const char* message);

} // namespace Internal

constexpr __forceinline void Assert(
  bool condition,
  const char* expression,
  const char* sourceFile,
  int sourceLine,
  const char* message
) {
  if (condition)
    return; // Assertation succeeded

  if consteval {
    // Assertation failed at compile-time. Throw an exception to trigger compilation error
    (void) expression;
    (void) sourceFile;
    (void) sourceLine;
    throw message;
  }
  else {
    // Assertation failed at run-time. Call runtime handler
    Internal::HandleAssertationFailure(expression, sourceFile, sourceLine, message);
  }
}

} // namespace MyGameEngine
