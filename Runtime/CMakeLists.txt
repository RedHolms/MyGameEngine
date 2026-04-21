cmake_minimum_required(VERSION 3.18)

include("CMake/PlatformInfo.cmake")

project(MyGameEngineRuntime
  VERSION 0.1.0.0
  LANGUAGES CXX ASM_NASM)

set(TARGET MyGameEngineRuntime)
add_library(${TARGET} STATIC)

glob_target_sources(${TARGET}
  PUBLIC "Include/*.hpp"
  PRIVATE "Source/*.cpp" "Source/*.hpp" "Source/*.asm"
)

collect_platform_info()

target_include_directories(${TARGET}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(GLOBAL_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Include/__stdafx.hpp")
target_precompile_headers(${TARGET} PUBLIC ${GLOBAL_HEADER_FILE})
target_compile_options(${TARGET} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/FI${GLOBAL_HEADER_FILE}>)

target_compile_definitions(${TARGET} PUBLIC
  _ALLOW_KEYWORD_MACROS=1
)

target_compile_options(${TARGET} PUBLIC
#  $<$<COMPILE_LANGUAGE:CXX>:-Wno-user-defined-literals -Wno-inline-new-delete>
)

target_link_options(${TARGET} PUBLIC
  /NODEFAULTLIB
  /ENTRY:__MyGameEngineRuntime_EntryPoint
  /OPT:REF,ICF
)

#add_subdirectory("Tests")
