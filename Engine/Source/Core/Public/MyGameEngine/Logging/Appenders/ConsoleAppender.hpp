#pragma once

#include <MyGameEngine/Logging/IAppender.hpp>

namespace Logging {

enum class ConsoleOutputFile {
  Standard, // stdout
  Error     // stderr
};

class ConsoleAppenderBuilder final {
public:
  constexpr ConsoleAppenderBuilder() = default;

public:
  ConsoleAppenderBuilder& SetFile(ConsoleOutputFile file) {
    m_file = file;
    return *this;
  }

  std::unique_ptr<IAppender> Build();

private:
  std::optional<ConsoleOutputFile> m_file;
};

} // namespace Logging
