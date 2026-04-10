cmake_minimum_required(VERSION 3.18)

set(TARGET MyGameEngine)
add_library(${TARGET} STATIC)

glob_target_sources(${TARGET}
  PUBLIC "Include/*.hpp"
  PRIVATE "Source/Common/*.cpp" "Source/Common/*.hpp"
)

target_link_libraries(${TARGET} PUBLIC MyGameEngineRuntime)