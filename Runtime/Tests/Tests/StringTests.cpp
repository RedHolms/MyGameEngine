#include "Core/String.hpp"

void shit(String const& arg);

bool TestBody() {
  auto constStr = "Hello, World!"s;
  shit(constStr);
  return true;
}
