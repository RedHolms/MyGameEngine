cmake_minimum_required(VERSION 3.18)

set(TARGET ExampleGame)
add_executable(${TARGET} WIN32)

glob_target_sources(${TARGET} PRIVATE "Source/*.hpp" "Source/*.cpp")

target_link_libraries(${TARGET} PRIVATE MyGameEngine)
