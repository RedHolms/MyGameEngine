cmake_minimum_required(VERSION 3.18)

project(ExampleGame
  VERSION 0.1.0.0
  LANGUAGES CXX)

# Force static runtimes
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# List of engine components we will need
set(MYGAMEENGINE_COMPONENTS
  Graphics/OpenGL
)

# Add the engine as subproject
add_subdirectory("../Engine" "${CMAKE_CURRENT_BINARY_DIR}/Engine")

# Register our example game
add_game(ExampleGame)

# Register primary game executable attached to the game
add_game_executable(ExampleGameExecutable GAME ExampleGame PRIMARY)

# Link engine components to the game's executable
target_link_libraries(ExampleGameExecutable PRIVATE
  MyGameEngine::Graphics::OpenGL
)

# Add source files to the game's executable
target_sources(ExampleGameExecutable PRIVATE
  "Source/main.cpp"
)

# Remove all unused symbols from result binary
target_link_options(ExampleGameExecutable PRIVATE "/OPT:REF,ICF")

## Add assets to the game
#add_game_assets(ExampleGame
#  ASSET "Shaders/hello-triangle.frag"
#    FROM FILE "Assets/Shaders/hello-triangle.frag"
#  ASSET "Shaders/hello-triangle.vert"
#    FROM FILE "Assets/Shaders/hello-triangle.vert"
#)
