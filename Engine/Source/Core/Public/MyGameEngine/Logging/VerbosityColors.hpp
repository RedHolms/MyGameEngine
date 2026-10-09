#pragma once

#include <MyGameEngine/Logging.hpp>

namespace Logging {

inline const char* GetVerbosityLevelANSIColor(VerbosityLevel level) {
  switch (level) {
#define _LOG_LEVEL(_NAME, NAME_UPPERCASE, ANSI_COLOR) case VerbosityLevel::NAME_UPPERCASE: return ANSI_COLOR;
#include <MyGameEngine/__logging_verbosity_levels.def>
#undef _LOG_LEVEL
  }

  return "\x1B[39m"; // Applies the default color
}

} // namespace Logging
