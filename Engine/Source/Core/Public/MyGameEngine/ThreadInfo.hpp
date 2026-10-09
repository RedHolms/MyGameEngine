#pragma once

#include <MyGameEngine/String.hpp>
#include <string>

struct ThreadInfo {
  uint32_t id;
  String name;

  // Same as "name" but encoded in UTF8. Useful for Logging so we don't need to re-encode stuff as much
  std::string nameUtf8;
};

namespace MyGameEngine {

ThreadInfo const& GetCurrentThreadInformation();

} // namespace MyGameEngine
