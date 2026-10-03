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

# __MyGameEngine_ComponentBase is the target that defines common properties for all engine components
add_library(__MyGameEngine_ComponentBase INTERFACE EXCLUDE_FROM_ALL)
target_compile_features(__MyGameEngine_ComponentBase INTERFACE cxx_std_26)

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

function(__my_game_engine_register_component COMPONENT_NAME)
  set(component_target "__MyGameEngine_Component_${COMPONENT_NAME}")

  if(TARGET ${component_target})
    # Already registered
    return()
  endif()

  set(component_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Source/${COMPONENT_NAME}")

  if(NOT IS_DIRECTORY ${component_dir} OR NOT EXISTS ${component_dir}/Component.cmake)
    message(FATAL_ERROR "Specified component doesn't exist: ${COMPONENT_NAME}")
  endif()

  message(VERBOSE "Registering component ${COMPONENT_NAME}...")
  message(DEBUG " Component directory: ${component_dir}")
  message(DEBUG " Component target: ${component_target}")

  include("${component_dir}/Component.cmake")

  if(DEFINED COMPONENT_PRIVATE_SOURCES)
    set(private_sources "${COMPONENT_PRIVATE_SOURCES}")
    message(DEBUG " COMPONENT_PRIVATE_SOURCES defined: ${private_sources}")
  else()
    unset(private_sources)
    message(DEBUG " COMPONENT_PRIVATE_SOURCES NOT defined")
  endif()

  if(DEFINED COMPONENT_PUBLIC_SOURCES)
    set(public_sources "${COMPONENT_PUBLIC_SOURCES}")
    message(DEBUG " COMPONENT_PUBLIC_SOURCES defined: ${public_sources}")
  else()
    unset(public_sources)
    message(DEBUG " COMPONENT_PUBLIC_SOURCES NOT defined")
  endif()

  if(DEFINED COMPONENT_PRIVATE_DEPENDENCIES)
    set(private_dependencies "${COMPONENT_PRIVATE_DEPENDENCIES}")
    message(DEBUG " COMPONENT_PRIVATE_DEPENDENCIES defined: ${private_dependencies}")
  else()
    unset(private_dependencies)
    message(DEBUG " COMPONENT_PRIVATE_DEPENDENCIES NOT defined")
  endif()

  if(DEFINED COMPONENT_PUBLIC_DEPENDENCIES)
    set(public_dependencies "${COMPONENT_PUBLIC_DEPENDENCIES}")
    message(DEBUG " COMPONENT_PUBLIC_DEPENDENCIES defined: ${public_dependencies}")
  else()
    unset(public_dependencies)
    message(DEBUG " COMPONENT_PUBLIC_DEPENDENCIES NOT defined")
  endif()

  # Delete them in case we call this function recursively (so no pollution occurs as components are not required to set
  # all variables)
  unset(COMPONENT_PRIVATE_SOURCES)
  unset(COMPONENT_PUBLIC_SOURCES)
  unset(COMPONENT_PRIVATE_DEPENDENCIES)
  unset(COMPONENT_PUBLIC_DEPENDENCIES)

  # These 2 may have paths relative to component dir. Make them absolute
  if(DEFINED private_sources)
    __my_game_engine_absolutize_paths_list("${private_sources}" "${component_dir}" private_sources)
  endif()

  if(DEFINED public_sources)
    __my_game_engine_absolutize_paths_list("${public_sources}" "${component_dir}" public_sources)
  endif()

  if(NOT DEFINED private_sources AND NOT DEFINED public_sources)
    message(FATAL_ERROR "Component ${COMPONENT_NAME} doesn't have any sources")
  endif()

  if(DEFINED private_sources)
    set(target_type STATIC)
    set(public_visibility PUBLIC)
  else()
    set(target_type INTERFACE)
    set(public_visibility INTERFACE)
  endif()

  message(DEBUG " Target type: ${target_type}")
  add_library(${component_target} ${target_type} EXCLUDE_FROM_ALL)

  ## Process private sources
  if(DEFINED private_sources)
    target_link_libraries(${component_target} PRIVATE __MyGameEngine_ComponentBase)
    target_include_directories(${component_target} PRIVATE ${component_dir})
    target_sources(${component_target} PRIVATE ${private_sources})
  endif()

  ## Process public sources
  if(DEFINED public_sources)
    target_include_directories(${component_target} ${public_visibility} "${component_dir}/Public")
    target_sources(${component_target} ${public_visibility} ${public_sources})
  endif()

  ## Process private dependencies
  if(DEFINED private_dependencies)
    if(NOT DEFINED private_sources)
      message(SEND_ERROR "Component ${COMPONENT_NAME} doesn't have any private sources but has private dependencies:
        ${private_dependencies}")
    else()
      foreach(dependency IN LISTS private_dependencies)
        __my_game_engine_register_component(${dependency})
        target_link_libraries(${component_target} PRIVATE __MyGameEngine_Component_${dependency})
      endforeach()
    endif()
  endif()

  ## Process public dependencies
  if(DEFINED public_dependencies)
    if(NOT DEFINED public_sources)
      message(SEND_ERROR "Component ${COMPONENT_NAME} doesn't have any public sources but has public dependencies:
        ${public_dependencies}")
    else()
      foreach(dependency IN LISTS public_dependencies)
        __my_game_engine_register_component(${dependency})
        target_link_libraries(${component_target} ${public_visibility} __MyGameEngine_Component_${dependency})
      endforeach()
    endif()
  endif()

  ## Expose component information into global scope
  if(DEFINED private_sources)
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_HAS_PRIVATE_SOURCES 1)
  else()
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_HAS_PRIVATE_SOURCES 0)
  endif()

  if(DEFINED public_sources)
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_HAS_PUBLIC_SOURCES 1)
  else()
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_HAS_PUBLIC_SOURCES 0)
  endif()

  if(DEFINED private_dependencies)
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_PRIVATE_DEPENDENCIES "${private_dependencies}")
  endif()

  if(DEFINED public_dependencies)
    __set_global(MYGAMEENGINE_${COMPONENT_NAME}_PUBLIC_DEPENDENCIES "${public_dependencies}")
  endif()
endfunction()

function(__my_game_engine_enable_component TARGET_NAME COMPONENT_NAME REASON)
  __my_game_engine_register_component(${COMPONENT_NAME})
  set(component_target "__MyGameEngine_Component_${COMPONENT_NAME}")
  target_link_libraries(${TARGET_NAME} PRIVATE ${component_target})
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
  target_compile_features(${TARGET_NAME} INTERFACE cxx_std_26)

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
