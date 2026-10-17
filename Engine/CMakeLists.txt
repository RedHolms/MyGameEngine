cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED MYGAMEENGINE_IS_MASTER_PROJECT)
  set(MYGAMEENGINE_IS_MASTER_PROJECT ON)
  if(DEFINED PROJECT_NAME)
    set(MYGAMEENGINE_IS_MASTER_PROJECT OFF)
  endif()
endif()

if(MYGAMEENGINE_IS_MASTER_PROJECT)
  message(FATAL_ERROR "There's no point in compiling MyGameEngine as standalone project for now")
endif()

project(MyGameEngine
  VERSION 0.1.0.0
  LANGUAGES CXX)

#
# Check the compiler & linker. Currently we require MSVC frontends for both compiler and linker. While MSVC might work
# we recommend to use Clang.
#

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  message(FATAL_ERROR "You must use a compiler with MSVC frontend. Current compiler is: ${CMAKE_CXX_COMPILER_ID} "
    "(with frontend: ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(WARNING "Recommended compiler is Clang. MyGameEngine may not work properly (your compiler is ${CMAKE_CXX_COMPILER_ID})")
endif()

if(NOT CMAKE_CXX_COMPILER_LINKER_ID STREQUAL "MSVC" AND NOT CMAKE_CXX_COMPILER_LINKER_FRONTEND_VARIANT STREQUAL "MSVC")
  message(FATAL_ERROR "You must use a linker with MSVC frontend. Current linker is: ${CMAKE_CXX_COMPILER_LINKER_ID} "
    "(with frontend: ${CMAKE_CXX_COMPILER_LINKER_FRONTEND_VARIANT})")
endif()

#
# Add third-party dependencies.
#

# fmtlib
# Used for string formatting (e.g. for logging).
set(FMT_MASTER_PROJECT OFF)
add_subdirectory("ThirdParty/fmtlib")

#
# Include some generic CMake utilities
#
include("CMake/Utilities.cmake")

#
# Include engine components
#

# Automatically added components
set(MYGAMEENGINE_MANDATORY_COMPONENTS "Engine;Launch")

# User-required components. Default to an empty list
if(NOT DEFINED MYGAMEENGINE_COMPONENTS)
  set(MYGAMEENGINE_COMPONENTS "")
endif()

block()
  set(binary_dir "${CMAKE_CURRENT_BINARY_DIR}")
  set_property(GLOBAL PROPERTY MYGAMEENGINE_REGISTERED_COMPONENTS "")

  function(_guarded_add_component COMPONENT_NAME)
    get_property(registered_components GLOBAL PROPERTY MYGAMEENGINE_REGISTERED_COMPONENTS)
    if(NOT "${COMPONENT_NAME}" IN_LIST registered_components)
      set(component_directory "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Source/${COMPONENT_NAME}")

      if(NOT IS_DIRECTORY "${component_directory}")
        message(FATAL_ERROR "Invalid component ${component_name}. Directory not found: ${component_directory}")
      endif()

      if(NOT EXISTS "${component_directory}/CMakeLists.txt")
        message(FATAL_ERROR "Invalid component ${component_name}. File not found: ${component_directory}/CMakeLists.txt")
      endif()

      cmake_path(RELATIVE_PATH component_directory
        BASE_DIRECTORY "${CMAKE_CURRENT_FUNCTION_LIST_DIR}"
        OUTPUT_VARIABLE component_directory_rel)

      set_property(GLOBAL APPEND PROPERTY MYGAMEENGINE_REGISTERED_COMPONENTS "${COMPONENT_NAME}")
      add_subdirectory("${component_directory}" "${binary_dir}/${component_directory_rel}")
    endif()
  endfunction()

  function(add_dependency_component COMPONENT_NAME)
    _guarded_add_component("${COMPONENT_NAME}")
  endfunction()

  foreach(component_name IN LISTS MYGAMEENGINE_MANDATORY_COMPONENTS MYGAMEENGINE_COMPONENTS)
    _guarded_add_component("${component_name}")
  endforeach()
endblock()

#
# Internal utilities
#

function(_mge_check_is_game_target TARGET)
  if(NOT TARGET "${TARGET}")
    message(FATAL_ERROR "${TARGET} is not a target")
  endif()

  get_target_property(is_game "${TARGET}" IS_GAME)

  if(NOT is_game)
    message(FATAL_ERROR "${TARGET} is not a game target")
  endif()
endfunction()

function(_mge_check_is_game_exe_target TARGET)
  if(NOT TARGET "${TARGET}")
    message(FATAL_ERROR "${TARGET} is not a target")
  endif()

  get_target_property(is_game_executable "${TARGET}" IS_GAME_EXECUTABLE)

  if(NOT is_game_executable)
    message(FATAL_ERROR "${TARGET} is not a game executable target")
  endif()
endfunction()

#
# add_game(<name>)
#
# Functions adds a new game target called <name>
#
function(add_game GAME_NAME)
  add_custom_target("${GAME_NAME}" ALL)
  set_target_properties("${GAME_NAME}" PROPERTIES
    IS_GAME TRUE
  )
endfunction()

#
# add_game_executable(<name> GAME <game-name> [PRIMARY])
#
# Function adds a new executable target called <name> (on Windows, target is added as WIN32 target) and attaches in to
# the game named <game-name>. If optional PRIMARY argument is present, executable is considered primary game executable.
# Exactly one executable can be game's primary executable at the time.
#
function(add_game_executable TARGET_NAME)
  cmake_parse_arguments(PARSE_ARGV 1 arg
    "PRIMARY"
    "GAME"
    ""
  )

  if(NOT DEFINED arg_GAME)
    message(FATAL_ERROR "GAME must be specified")
  endif()

  _mge_check_is_game_target("${arg_GAME}")

  get_target_property(primary_target "${arg_GAME}" GAME_PRIMARY_EXECUTABLE)
  if(primary_target)
    message(FATAL_ERROR "${arg_GAME} already has a primary executable ${primary_target}")
  endif()

  add_executable("${TARGET_NAME}" WIN32)

  target_compile_features("${TARGET_NAME}" PRIVATE cxx_std_26)
  target_link_libraries("${TARGET_NAME}" PRIVATE
    MyGameEngine
    MyGameEngine::Launch
  )

  set_target_properties("${TARGET_NAME}" PROPERTIES
    IS_GAME_EXECUTABLE TRUE
  )

  set_target_properties("${arg_GAME}" PROPERTIES
    GAME_PRIMARY_EXECUTABLE "${TARGET_NAME}"
  )

  add_dependencies("${arg_GAME}" "${TARGET_NAME}")
endfunction()
