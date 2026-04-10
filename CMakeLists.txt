cmake_minimum_required(VERSION 3.18)

include("CMake/Utilities.cmake")

project(MyGameEngine
  VERSION 0.1.0.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("Runtime")
add_subdirectory("Engine")

# Example game project
# TODO: include only if top-level project
add_subdirectory("ExampleGame")
