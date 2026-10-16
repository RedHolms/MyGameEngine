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

# Static and build-generated files can be mixed in one archive. The generator script is tracked in the project, while
# its output lives in the build tree; the archive target depends on that output automatically.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(generated_asset "${CMAKE_CURRENT_BINARY_DIR}/GeneratedAssets/build-info.txt")
add_custom_command(
  OUTPUT "${generated_asset}"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/Build/GenerateBuildInfo.py" "${generated_asset}"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Build/GenerateBuildInfo.py"
  COMMENT "Generating example asset"
  VERBATIM
)
add_custom_target(GenerateBuildInfo DEPENDS "${generated_asset}")
MyGameEngine_AddAsset(TARGET ExampleGame ARCHIVE game FILE "Assets/welcome.txt" AS "docs/welcome.txt")
MyGameEngine_AddAsset(TARGET ExampleGame ARCHIVE game FILE "${generated_asset}" AS "generated/build-info.txt"
  GENERATED_BY GenerateBuildInfo)

# Remove all unused symbols from result binary
target_link_options(ExampleGame PRIVATE "/OPT:REF,ICF")
