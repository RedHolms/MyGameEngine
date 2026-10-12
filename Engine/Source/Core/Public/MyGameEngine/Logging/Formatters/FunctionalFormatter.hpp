#pragma once

#include <MyGameEngine/Logging/IFormatter.hpp>
#include <functional>

namespace Logging {

class FunctionalFormatterBuilder final {
private:
  using FormatterFunction = std::function<std::string(LoggingRecord const&, FormattingContext const&)>;

public:
  constexpr FunctionalFormatterBuilder() = default;

public:
  FunctionalFormatterBuilder& SetFunction(FormatterFunction&& function) {
    m_formatter = std::move(function);
    return *this;
  }

  std::unique_ptr<IFormatter> Build();

private:
  FormatterFunction m_formatter;
};

} // namespace Logging
