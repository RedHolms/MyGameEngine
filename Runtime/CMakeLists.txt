cmake_minimum_required(VERSION 3.18)

project(MyGameEngineRuntime
  VERSION 0.1.0.0
  LANGUAGES CXX ASM_MASM)

set(TARGET MyGameEngineRuntime)
add_library(${TARGET} STATIC)

file(GLOB_RECURSE HEADERS "Include/*.hpp")
target_sources(${TARGET} PUBLIC ${HEADERS})

file(GLOB_RECURSE SOURCES "Source/Common/*.cpp" "Source/Common/*.hpp")

try_run(_ _
  SOURCE_FROM_FILE "platform-info.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Extras/platform-info.cpp"
  CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_SOURCE_DIR}/Include"
  RUN_OUTPUT_VARIABLE PLATFORM_INFO
)

list(GET PLATFORM_INFO 0 TARGET_OS)
list(GET PLATFORM_INFO 1 TARGET_OS_NAME)
list(GET PLATFORM_INFO 2 TARGET_ABI_FAMILY)
list(GET PLATFORM_INFO 3 TARGET_ABI_FAMILY_NAME)
list(GET PLATFORM_INFO 4 TARGET_ARCH)
list(GET PLATFORM_INFO 5 TARGET_ARCH_NAME)

message(STATUS "Target OS: ${TARGET_OS_NAME}")
message(STATUS "Target ABI Family: ${TARGET_ABI_FAMILY_NAME}")
message(STATUS "Target Architecture: ${TARGET_ARCH_NAME}")

if(TARGET_OS STREQUAL "WINDOWS")
  file(GLOB_RECURSE OS_SOURCES "Source/OS/Win32/*.cpp" "Source/OS/Win32/*.hpp")
endif()

if(TARGET_ARCH STREQUAL "AMD64")
  file(GLOB_RECURSE ARCH_SOURCES "Source/Arch/AMD64/*.asm")
endif()

target_sources(${TARGET} PRIVATE ${SOURCES} ${OS_SOURCES} ${ARCH_SOURCES})

target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

set(GLOBAL_HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Include/StdAfx.hpp")
target_precompile_headers(${TARGET} PUBLIC ${GLOBAL_HEADER_FILE})
target_compile_options(${TARGET} PUBLIC
  /FI${GLOBAL_HEADER_FILE}
  /EHs-c-
)

target_link_options(${TARGET} PUBLIC
  /NODEFAULTLIB
  /ENTRY:__MyGameEngineRuntime_EntryPoint
)

add_subdirectory("Tests")
