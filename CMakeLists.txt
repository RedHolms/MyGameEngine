cmake_minimum_required(VERSION 3.18)

project(MyGameEngine
  VERSION 0.1.0.0
  LANGUAGES CXX)

file(GLOB_RECURSE SOURCES
  "Source/*.cpp" "Source/*.hpp" "Source/*.c" "Source/*.h"
  "Source/*.asm"
)
add_executable(Game WIN32 ${SOURCES})

target_include_directories(Game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

set_target_properties(Game PROPERTIES
  CXX_STANDARD 20
)

target_compile_options(Game PRIVATE
  /GS            # enable custom security cookie
  /Zl            # omit default library name
  /GR-           # no RTTI
)

set(GLOBAL_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/StdAfx.hpp")
target_precompile_headers(Game PRIVATE ${GLOBAL_HEADER_FILE})
target_compile_options(Game PRIVATE "/FI${GLOBAL_HEADER_FILE}")

target_link_options(Game PRIVATE
  /NODEFAULTLIB
  /ENTRY:__MyGameEngine_EntryPoint
)

