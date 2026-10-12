#pragma once

#include <MyGameEngine/Logging.hpp>

namespace Logging {

struct FormattingContext {
  // Target we formatting to supports ANSI escape sequences (mainly used for colors)
  bool supportsEscapeSequences;
};

class IFormatter {
public:
  virtual ~IFormatter() = 0;

public:
  /**
   * This function must convert given record to a string. Result string must not contain a line break at the end. Also,
   * formatting context is passed. This context may change the behaviour of formatting (e.g. enabling colors for the
   * terminal). This function **MUST** return **UTF-8** string.
   */
  virtual std::string FormatRecord(LoggingRecord const& record, FormattingContext const& context) = 0;
};

} // namespace Logging
