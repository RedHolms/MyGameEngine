#include "Core/String.hpp"

extern bool TestBody();

void shit(String const& arg) {}

uint Main() {
  // Run the test body
  return TestBody() ? 0 : 1;
}
