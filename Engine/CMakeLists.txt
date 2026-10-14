cmake_minimum_required(VERSION 3.25)

set(MYGAMEENGINE_IS_MASTER_PROJECT ON)
if(DEFINED PROJECT_NAME)
  # Some other project had been already defined
  set(MYGAMEENGINE_IS_MASTER_PROJECT OFF)
endif()

project(MyGameEngine
  VERSION 0.1.0.0
  LANGUAGES CXX ASM_NASM)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "You must use Clang compiler. Current is: ${CMAKE_CXX_COMPILER_ID}")
endif()

## Add third-party dependencies

# fmtlib
# Used for logging system and other formatting stuff
set(FMT_MASTER_PROJECT OFF)
add_subdirectory("ThirdParty/fmtlib")

# __MyGameEngine_ComponentBase is the target that defines common properties for all engine components
add_library(__MyGameEngine_ComponentBase INTERFACE EXCLUDE_FROM_ALL)
target_compile_features(__MyGameEngine_ComponentBase INTERFACE cxx_std_26)

set(MYGAMEENGINE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "MYGAMEENGINE_BINARY_DIR")
set(MYGAMEENGINE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "MYGAMEENGINE_SOURCE_DIR")

macro(__set_global NAME VALUE)
  set("${NAME}" "${VALUE}" CACHE INTERNAL "${NAME}")
endmacro()

function(__my_game_engine_absolutize_paths_list PATHS_LIST BASE_DIR OUTPUT_VAR)
  set(result "")

  foreach(PATH IN LISTS PATHS_LIST)
    if(IS_ABSOLUTE PATH)
      set(abs_path "${PATH}")
    else()
      cmake_path(ABSOLUTE_PATH PATH BASE_DIRECTORY "${BASE_DIR}" OUTPUT_VARIABLE abs_path)
    endif()
    list(APPEND result "${abs_path}")
  endforeach()

  set("${OUTPUT_VAR}" "${result}" PARENT_SCOPE)
endfunction()

function(__my_game_engine_validate_component_name COMPONENT_NAME OUTPUT_VAR)
  if(COMPONENT_NAME MATCHES "[A-Za-z0-9_.\\/]")
    set(result 1)
  else()
    set(result 0)
  endif()

  set("${OUTPUT_VAR}" "${result}" PARENT_SCOPE)
endfunction()

function(__my_game_engine_get_component_target COMPONENT_NAME OUTPUT_VAR)
  set(result "__MyGameEngine_Component_${COMPONENT_NAME}")
  string(REGEX REPLACE "[\\/]" "_" result "${result}")
  set("${OUTPUT_VAR}" "${result}" PARENT_SCOPE)
endfunction()

function(__my_game_engine_link_component TARGET_NAME VISIBILITY COMPONENT_NAME)
  __my_game_engine_register_component(${COMPONENT_NAME})
  __my_game_engine_get_component_target(${COMPONENT_NAME} link_target)

  if(MYGAMEENGINE_${COMPONENT_NAME}_REQUIRES_FULL_LINKAGE)
    target_link_libraries(${TARGET_NAME} ${VISIBILITY} $<LINK_LIBRARY:WHOLE_ARCHIVE,${link_target}>)
  else()
    target_link_libraries(${TARGET_NAME} ${VISIBILITY} ${link_target})
  endif()
endfunction()

function(__my_game_engine_register_component COMPONENT_NAME)
  __my_game_engine_get_component_target(${COMPONENT_NAME} component_target)

  if(TARGET ${component_target})
    # Already registered
    return()
  endif()

  set(component_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Source/${COMPONENT_NAME}")

  if(NOT IS_DIRECTORY ${component_dir} OR NOT EXISTS ${component_dir}/CMakeLists.txt)
    message(FATAL_ERROR "Specified component doesn't exist: ${COMPONENT_NAME}")
  endif()

  message(VERBOSE "Entering component ${COMPONENT_NAME} directory...")

  string(REGEX REPLACE "[\\/]" "_" component_name_normalized "${COMPONENT_NAME}")

  set(MYGAMEENGINE_CURRENT_COMPONENT_NAME "${COMPONENT_NAME}")
  set(TARGET "${component_target}")
  add_subdirectory(${component_dir} "${MYGAMEENGINE_BINARY_DIR}/Components/${component_name_normalized}")
  unset(TARGET)
  unset(MYGAMEENGINE_CURRENT_COMPONENT_NAME)

  if(NOT TARGET ${component_target})
    message(FATAL_ERROR "CMakeLists.txt of ${COMPONENT_NAME} component did not register anything")
  endif()
endfunction()

function(MyGameEngine_DefineEngineComponent)
  set(args ${ARGV})

  # Throws an error if "args" is empty list. If "args" is not empty, pops the first element to "arg" variable
  macro(_pop_arg)
    list(LENGTH args args_len)
    if(args_len EQUAL 0)
      message(FATAL_ERROR "Unexpected end of arguments")
    endif()
    list(POP_FRONT args arg)
  endmacro()

  # If "args" is empty list, only sets "has_arg" to FALSE. If "args" is not empty, sets "has_arg" to TRUE and sets "arg"
  # to first item of "args"
  macro(_peek_arg)
    list(LENGTH args args_len)
    if(args_len EQUAL 0)
      set(has_arg FALSE)
    else()
      set(has_arg TRUE)
      list(GET args 0 arg)
    endif()
  endmacro()

  set(CONTROL_WORDS "TYPE;DEPENDS_ON;REQUIRE_FULL_LINKAGE")

  unset(component_type)
  set(dependencies "")
  set(require_full_linkage "FALSE")

  macro(_ctrl_TYPE)
    if(DEFINED component_type)
      message(FATAL_ERROR "TYPE defined multiple times")
    endif()

    _pop_arg()
    set(component_type "${arg}")
  endmacro()

  macro(_ctrl_DEPENDS_ON)
    _peek_arg()
    while(has_arg AND NOT arg IN_LIST CONTROL_WORDS)
      _pop_arg()
      list(APPEND dependencies "${arg}")
      _peek_arg()
    endwhile()
  endmacro()

  macro(_ctrl_REQUIRE_FULL_LINKAGE)
    _pop_arg()
    set(require_full_linkage "${arg}")
  endmacro()

  while(args)
    list(POP_FRONT args arg)

    if(NOT arg IN_LIST CONTROL_WORDS)
      message(FATAL_ERROR "Invalid command arguments: ${arg} is unexpected")
    endif()

    cmake_language(CALL "_ctrl_${arg}")
  endwhile()

  if(NOT DEFINED component_type)
    message(FATAL_ERROR "TYPE is not defined")
  endif()

  set(can_have_private_sources TRUE)
  set(public_vis PUBLIC)

  if(component_type STREQUAL "COMPILABLE_STATIC")
    add_library(${TARGET} STATIC EXCLUDE_FROM_ALL)
    target_link_libraries(${TARGET} PRIVATE __MyGameEngine_ComponentBase)
    target_compile_definitions(${TARGET} PRIVATE MYGAMEENGINE_CURRENT_COMPONENT_NAME="${MYGAMEENGINE_CURRENT_COMPONENT_NAME}")
  endif()

  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Public")
    target_include_directories(${TARGET} ${public_vis} "${CMAKE_CURRENT_LIST_DIR}/Public")
  endif()

  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Private")
    if(NOT can_have_private_sources)
      message(FATAL_ERROR "Current component TYPE does not support private sources")
    endif()
    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
  endif()

  foreach(dependency IN LISTS dependencies)
    __my_game_engine_link_component(${TARGET} ${public_vis} ${dependency})
  endforeach()

  __set_global(MYGAMEENGINE_${MYGAMEENGINE_CURRENT_COMPONENT_NAME}_REQUIRES_FULL_LINKAGE "${require_full_linkage}")
endfunction()

function(__my_game_engine_enable_component TARGET_NAME COMPONENT_NAME REASON)
  __my_game_engine_link_component(${TARGET_NAME} PRIVATE ${COMPONENT_NAME})
endfunction()

#
# MyGameEngine_AddGame(
#   <TARGET_NAME>
#   [ADDITIONAL_COMPONENTS
#     <COMPONENT_NAME_1>
#     <COMPONENT_NAME_2>...]
# )
#
# Creates new executable target with the name <TARGET_NAME>. Automatically links that target to the engine and enables
# 2 components: 'Engine' and 'Launch'. You can specify more components to enable using 'ADDITIONAL_COMPONENTS'.
#
function(MyGameEngine_AddGame TARGET_NAME)
  add_executable(${TARGET_NAME} WIN32)
  target_compile_features(${TARGET_NAME} PRIVATE cxx_std_26)

  __my_game_engine_enable_component(${TARGET_NAME} Engine "core component")
  __my_game_engine_enable_component(${TARGET_NAME} Launch "core component")

  set(state_modifiers "ADDITIONAL_COMPONENTS")

  set(current_state "")

  foreach(arg IN LISTS ARGN)
    if(arg IN_LIST state_modifiers)
      set(current_state "${arg}")
      continue()
    endif()

    if(current_state STREQUAL "ADDITIONAL_COMPONENTS")
      __my_game_engine_enable_component(${TARGET_NAME} ${arg} "added by user")
    endif()
  endforeach()
endfunction()

# Utility function.
# Creates custom command that will create ".gitignore" file in the target directory ignoring everything in it.
# Custom command outputs "${DIRECTORY_PATH}/.gitignore" file
function(MyGameEngine_MakeGeneratedDirectory DIRECTORY_PATH)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_custom_command(
    OUTPUT "${DIRECTORY_PATH}/.gitignore"
    COMMAND
      ${Python3_EXECUTABLE} "${MYGAMEENGINE_SOURCE_DIR}/Build/make-generated-directory.py" "${DIRECTORY_PATH}"
    COMMENT
      "Making generated directory ${DIRECTORY_PATH}..."
  )
endfunction()
