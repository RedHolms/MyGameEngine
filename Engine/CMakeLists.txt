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

  add_custom_target("${GAME_NAME}_Assets")
  add_dependencies("${GAME_NAME}" "${GAME_NAME}_Assets")
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

#
# add_game_assets(<game-name> <archive-name>
#   {ASSET <asset-name> FROM
#     {FILE <file-path>}
#   } ...
# )
#
# Function adds a new archive called <archive-name> to the game called <game-name>. This archive will contain specified
# assets. <archive-name> must be a valid file name. <asset-name> can be any string. In the release bundle, file named
# "<archive-name>.gamefiles" will be created in the game files. For now, the only way to add assets is importing them
# from files on the dev's PC.
#
function(add_game_assets_archive GAME_NAME ARCHIVE_NAME)
  set(args ${ARGN})
  list(LENGTH args args_left)

  macro(pop_arg)
    if(args_left GREATER 0)
      set(has_arg TRUE)
      list(POP_FRONT args arg)
      math(EXPR args_left "${args_left}-1")
    else()
      set(has_arg FALSE)
    endif()
  endmacro()

  macro(pop_keyword)
    set(possible_variants ${ARGV})
    pop_arg()
    if(NOT has_arg OR NOT arg IN_LIST possible_variants)
      message(FATAL_ERROR "Expected one of: ${possible_variants}")
    endif()
  endmacro()

  set(archive_bin_dir "${CMAKE_CURRENT_BINARY_DIR}/${GAME_NAME}/assetsbuild/${ARCHIVE_NAME}")
  file(MAKE_DIRECTORY "${archive_bin_dir}")

  set(archive_build_cmd_dependencies "")

  set(archive_def "{}")

  string(JSON encoded STRING_ENCODE "${GAME_NAME}")
  string(JSON archive_def SET "${archive_def}" "game-name" "${encoded}")

  string(JSON encoded STRING_ENCODE "${ARCHIVE_NAME}")
  string(JSON archive_def SET "${archive_def}" "archive-name" "${encoded}")

  set(assets_list "[]")

  while(args_left GREATER 0)
    pop_keyword(ASSET)

    pop_arg()
    if(NOT has_arg)
      message(FATAL_ERROR "Asset name expected")
    endif()

    set(asset_name "${arg}")

    pop_keyword(FROM)
    pop_keyword(FILE)

    pop_arg()
    if(NOT has_arg)
      message(FATAL_ERROR "Asset file path expected")
    endif()

    cmake_path(ABSOLUTE_PATH arg NORMALIZE OUTPUT_VARIABLE asset_file_path)
    cmake_path(NATIVE_PATH asset_file_path NORMALIZE asset_file_path)

    list(APPEND archive_build_cmd_dependencies "${asset_file_path}")

    set(asset_def "{}")

    string(JSON encoded STRING_ENCODE "${asset_name}")
    string(JSON asset_def SET "${asset_def}" "name" "${encoded}")

    string(JSON asset_def SET "${asset_def}" "source" "\"file\"")

    string(JSON encoded STRING_ENCODE "${asset_file_path}")
    string(JSON asset_def SET "${asset_def}" "file-path" "${encoded}")

    string(JSON assets_list SET "${assets_list}" 99999999 "${asset_def}")
  endwhile()

  string(JSON archive_def SET "${archive_def}" "assets" "${assets_list}")

  set(def_file_path "${archive_bin_dir}/definition.json")
  set(temp_def_file_path "${archive_bin_dir}/definition.json.tmp")

  file(WRITE "${temp_def_file_path}" "${archive_def}")
  # Wont touch the file if it's the same so we won't repack the archive after each cmake reconfiguration
  configure_file("${temp_def_file_path}" "${def_file_path}" COPYONLY)

  set(result_archive_path "${archive_bin_dir}/result.gamefiles")

  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_custom_command(
    OUTPUT "${result_archive_path}"
    COMMAND
      ${Python3_EXECUTABLE}
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Build/AssetsPak/generate-assets-archive.py"
        "${result_archive_path}"
        "${def_file_path}"
    DEPENDS
      ${archive_build_cmd_dependencies}
      "${def_file_path}"
    COMMENT
      "Packing assets to the archive \"${ARCHIVE_NAME}\" (game: ${GAME_NAME})..."
  )

  add_custom_target("${GAME_NAME}_BuildArchive_${ARCHIVE_NAME}" DEPENDS "${result_archive_path}")
  add_dependencies("${GAME_NAME}_Assets" "${GAME_NAME}_BuildArchive_${ARCHIVE_NAME}")

  # FIXME FIXME FIXME!!!
  # For now let's just copy the archive to the primary executable. Should be changed once we figure out a proper
  # directory structure for result game bundle.

  get_target_property(primary_executable_target "${GAME_NAME}" GAME_PRIMARY_EXECUTABLE)

  # FIXME: Make it so it doesn't run EACH fking time
  add_custom_command(
    TARGET "${GAME_NAME}"
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
        "${result_archive_path}"
        "$<TARGET_FILE_DIR:${primary_executable_target}>/${ARCHIVE_NAME}.gamefiles"
    COMMAND_EXPAND_LISTS
    COMMENT
      "Copying assets (\"${ARCHIVE_NAME}\") to the primary game executable..."
  )
endfunction()
