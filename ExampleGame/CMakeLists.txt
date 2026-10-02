cmake_minimum_required(VERSION 3.18)

project(ExampleGame
  VERSION 0.1.0.0
  LANGUAGES CXX)

# Force static runtimes
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Remove all unused symbols from result binary
add_link_options("/OPT:REF,ICF")

add_subdirectory("../Engine" "${CMAKE_CURRENT_BINARY_DIR}/Engine")

add_executable(ExampleGame WIN32)
add_subdirectory("Source")
target_link_libraries(ExampleGame PRIVATE MyGameEngine)
