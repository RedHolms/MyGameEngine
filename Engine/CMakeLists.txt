cmake_minimum_required(VERSION 3.18)

set(TARGET MyGameEngine)
add_library(${TARGET} STATIC)

glob_target_sources(${TARGET}
  PUBLIC "Include/*.hpp"
  PRIVATE "Source/*.cpp" "Source/*.hpp"
)

target_include_directories(${TARGET}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

target_link_libraries(${TARGET} PUBLIC MyGameEngineRuntime)