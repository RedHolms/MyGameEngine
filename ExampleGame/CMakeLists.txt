cmake_minimum_required(VERSION 3.18)

project(ExampleGame
  VERSION 0.1.0.0
  LANGUAGES CXX)

# Force static runtimes
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory("../Engine" "${CMAKE_CURRENT_BINARY_DIR}/Engine")

MyGameEngine_AddGame(
  ExampleGame

  ADDITIONAL_COMPONENTS
    Graphics/OpenGL
)

add_subdirectory("Source")

# Remove all unused symbols from result binary
target_link_options(ExampleGame PRIVATE "/OPT:REF,ICF")
