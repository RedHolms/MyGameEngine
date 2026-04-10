cmake_minimum_required(VERSION 3.18)

include("CMake/PlatformInfo.cmake")

project(MyGameEngineRuntime
  VERSION 0.1.0.0
  LANGUAGES CXX ASM_MASM)

set(TARGET MyGameEngineRuntime)
add_library(${TARGET} STATIC)

glob_target_sources(${TARGET}
  PUBLIC "Include/*.hpp"
  PRIVATE "Source/Common/*.cpp" "Source/Common/*.hpp"
)

collect_platform_info()

if(TARGET_OS STREQUAL "WINDOWS")
  glob_target_sources(${TARGET} PRIVATE "Source/OS/Win32/*.cpp" "Source/OS/Win32/*.hpp")
endif()

if(TARGET_ARCH STREQUAL "AMD64")
  glob_target_sources(${TARGET} PRIVATE "Source/Arch/AMD64/*.asm")
endif()

target_include_directories(${TARGET}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(GLOBAL_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Include/StdAfx.hpp")
target_precompile_headers(${TARGET} PUBLIC ${GLOBAL_HEADER_FILE})
target_compile_options(${TARGET} PUBLIC /FI${GLOBAL_HEADER_FILE})

target_compile_options(${TARGET} PUBLIC
  /EHs-c- # no exceptions
)

target_link_options(${TARGET} PUBLIC
  /NODEFAULTLIB
  /ENTRY:__MyGameEngineRuntime_EntryPoint
)

add_subdirectory("Tests")
