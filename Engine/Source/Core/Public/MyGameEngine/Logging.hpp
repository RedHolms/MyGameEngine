#pragma once

#include <MyGameEngine/ThreadInfo.hpp>
#include <fmt/format.h>
#include <chrono>
#include <string>
#include <source_location>

#ifndef MYGAMEENGINE_LOGGING_TRANSLATION_UNIT_CATEGORY

#ifdef MYGAMEENGINE_CURRENT_COMPONENT_NAME
// We're inside one of engine components
#define MYGAMEENGINE_LOGGING_TRANSLATION_UNIT_CATEGORY MYGAMEENGINE_CURRENT_COMPONENT_NAME
#else
// We're inside the game
#define MYGAMEENGINE_LOGGING_TRANSLATION_UNIT_CATEGORY "Game"
#endif

#endif // #ifndef MYGAMEENGINE_LOGGING_TRANSLATION_UNIT_CATEGORY

enum class VerbosityLevel {
#define _LOG_LEVEL(_NAME, NAME_UPPERCASE, _ANSI_COLOR) NAME_UPPERCASE,
#include <MyGameEngine/__logging_verbosity_levels.def>
#undef _LOG_LEVEL
};

struct LoggingRecord {
  VerbosityLevel verbosityLevel;
  uint32_t threadId;
  std::string threadNameUtf8;
  std::string textUtf8;
  std::string categoryUtf8;
  std::chrono::system_clock::time_point timepoint;
  std::source_location source;
};

namespace Logging {

void DispatchRecord(LoggingRecord&& record);

} // namespace Logging

namespace Log {

#define _LOG_LEVEL(NAME, NAME_UPPERCASE, _ANSI_COLOR)                                                                  \
  template <typename... ArgsT>                                                                                         \
  struct NAME final {                                                                                                  \
    __forceinline NAME(fmt::format_string<ArgsT...> fmt, ArgsT&&... args,                                              \
      std::source_location const source = std::source_location::current())                                             \
    {                                                                                                                  \
      auto const& currentThreadInfo = MyGameEngine::GetCurrentThreadInformation();                                     \
      Logging::DispatchRecord(LoggingRecord {                                                                          \
        .verbosityLevel = VerbosityLevel::NAME_UPPERCASE,                                                              \
        .threadId = currentThreadInfo.id,                                                                              \
        .threadNameUtf8 = currentThreadInfo.nameUtf8,                                                                  \
        .textUtf8 = fmt::format(fmt, std::forward<ArgsT>(args)...),                                                    \
        .categoryUtf8 = MYGAMEENGINE_LOGGING_TRANSLATION_UNIT_CATEGORY,                                                \
        .timepoint = std::chrono::system_clock::now(),                                                                 \
        .source = source                                                                                               \
      });                                                                                                              \
    }                                                                                                                  \
  };                                                                                                                   \
  template <typename... ArgsT> NAME(auto fmt, ArgsT&&...) -> NAME<ArgsT...>;
#include <MyGameEngine/__logging_verbosity_levels.def>
#undef _LOG_LEVEL

} // namespace Log
