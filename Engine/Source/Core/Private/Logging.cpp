#include <MyGameEngine/Logging.hpp>

#include <fmt/chrono.h>

void Logging::DispatchRecord(LoggingRecord&& record) {
  using namespace std::chrono;

  auto timepointLocalized = floor<nanoseconds>(zoned_time(current_zone(), record.timepoint).get_local_time());

  fmt::print("[{:%H:%M:%S}] {}\n", timepointLocalized, record.textUtf8);
}
