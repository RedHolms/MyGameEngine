#pragma once

#include <MyGameEngine/Logging.hpp>

namespace Logging {

class IAppender {
public:
  virtual ~IAppender() = 0;

public:
  /**
   * This function should return "true" if this appender prints formatted records to a terminal that supports ANSI
   * escape sequences ("\x1B[...")
   */
  virtual bool SupportsEscapeSequences() = 0;

  /**
   * Normally this function should return "true".
   *
   * Function may return "false" if this appender doesn't need formatted text and instead it uses raw record (e.g.
   * saving records to memory / raw serialization).
   */
  virtual bool NeedsFormatting() = 0;

  /**
   * This function must write/print the formatted record. "formattedLine" will contain result from some formatter. This
   * string WONT have any line break at the end.
   *
   * If NeedsFormatting() returns "false", "formattedLine" will be an empty string.
   */
  virtual void AppendFormattedRecord(LoggingRecord const& record, String const& formattedLine) = 0;
};

} // namespace Logging
