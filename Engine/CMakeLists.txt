cmake_minimum_required(VERSION 3.18)

# Check if we're the root project (will automatically enable tests)
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

# Base target all components must be depending on
add_library(_MyGameEngineBase INTERFACE EXCLUDE_FROM_ALL)
target_compile_features(_MyGameEngineBase INTERFACE cxx_std_26)

set(MYGAMEENGINE_COMPONENTS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")

function(_MyGameEngine_PushItemToCacheList LIST_VAR_NAME ITEM)
  if(NOT DEFINED ${LIST_VAR_NAME})
    set(CACHE{${LIST_VAR_NAME}} VALUE ${ITEM})
  else()
    set(_TEMP "${${LIST_VAR_NAME}}")
    list(APPEND _TEMP ${ITEM})
    set(CACHE{${LIST_VAR_NAME}} VALUE ${_TEMP})
  endif()
endfunction()

function(_MyGameEngine_DeclareComponent NAME)
  set(_COMPONENT_DIR "${MYGAMEENGINE_COMPONENTS_ROOT_DIR}/${NAME}")

  if(NOT IS_DIRECTORY "${_COMPONENT_DIR}" OR NOT EXISTS "${_COMPONENT_DIR}/CMakeLists.txt")
    message(SEND_ERROR "Invalid component ${NAME}")
    return()
  endif()

  set(_TARGET_NAME "_MyGameEngineComponent_${NAME}")

  set(_TARGET_TYPE STATIC)
  set(CURRENT_COMPONENT_IS_INTERFACE 0)
  if(NOT IS_DIRECTORY "${_COMPONENT_DIR}/Private")
    set(_TARGET_TYPE INTERFACE)
    set(CURRENT_COMPONENT_IS_INTERFACE 1)
  endif()

  if(_TARGET_TYPE STREQUAL "INTERFACE")
    add_library(${_TARGET_NAME} INTERFACE EXCLUDE_FROM_ALL)
    target_include_directories(${_TARGET_NAME} INTERFACE "${_COMPONENT_DIR}/Public")
  else()
    add_library(${_TARGET_NAME} STATIC EXCLUDE_FROM_ALL)
    target_include_directories(${_TARGET_NAME} PRIVATE "${_COMPONENT_DIR}")
    target_link_libraries(${_TARGET_NAME} PRIVATE _MyGameEngineBase)

    if(IS_DIRECTORY "${_COMPONENT_DIR}/Public")
      target_include_directories(${_TARGET_NAME} PUBLIC "${_COMPONENT_DIR}/Public")
    endif()
  endif()

  set(CACHE{MYGAMEENGINE_${NAME}_COMPONENT_EXPOSED_DEPENDENCIES} TYPE INTERNAL FORCE VALUE "")

  set(CURRENT_COMPONENT_TARGET ${_TARGET_NAME})
  set(CURRENT_COMPONENT_NAME ${NAME})
  add_subdirectory(${_COMPONENT_DIR})
  unset(CURRENT_COMPONENT_NAME)
  unset(CURRENT_COMPONENT_TARGET)
  unset(CURRENT_COMPONENT_IS_INTERFACE)
endfunction()

function(MyGameEngine_ComponentSources)
  if(NOT DEFINED CURRENT_COMPONENT_TARGET)
    message(SEND_ERROR "MyGameEngine_ComponentSources() must only be used inside engine components")
    return()
  endif()

  target_sources(${CURRENT_COMPONENT_TARGET} ${ARGV})
endfunction()

function(MyGameEngine_ComponentDependencies)
  if(NOT DEFINED CURRENT_COMPONENT_TARGET)
    message(SEND_ERROR "MyGameEngine_ComponentDependencies() must only be used inside engine components")
    return()
  endif()

  foreach(arg IN LISTS ARGV)
    if(arg STREQUAL "PUBLIC")
      if(CURRENT_COMPONENT_IS_INTERFACE)
        set(_CURRENT_SCOPE INTERFACE)
      else()
        set(_CURRENT_SCOPE PUBLIC)
      endif()
      continue()
    elseif(arg STREQUAL "PRIVATE")
      if(CURRENT_COMPONENT_IS_INTERFACE)
        message(SEND_ERROR "Cannot add PRIVATE source files to interface component")
        return()
      endif()

      set(_CURRENT_SCOPE PRIVATE)
      continue()
    elseif(NOT DEFINED _CURRENT_SCOPE)
      message(SEND_ERROR "Invalid MyGameEngine_ComponentDependencies() usage: No scope specified")
      return()
    endif()

    target_link_libraries(${CURRENT_COMPONENT_TARGET} ${_CURRENT_SCOPE} _MyGameEngineComponent_${arg})
  endforeach()
  unset(_CURRENT_SCOPE)
endfunction()

# All components go here:
_MyGameEngine_DeclareComponent("Core")
_MyGameEngine_DeclareComponent("Engine")
_MyGameEngine_DeclareComponent("Launch")
_MyGameEngine_DeclareComponent("Win32API")

# User target
add_library(MyGameEngine INTERFACE EXCLUDE_FROM_ALL)
target_compile_features(MyGameEngine INTERFACE cxx_std_26)

function(MyGameEngine_EnableComponent NAME)
  if(NOT TARGET _MyGameEngineComponent_${NAME})
    message(SEND_ERROR "Engine component with name ${NAME} doesn't exist")
    return()
  endif()

  target_link_libraries(MyGameEngine INTERFACE _MyGameEngineComponent_${NAME})
endfunction()

function(MyGameEngine_EnableComponents)
  foreach(arg IN LISTS ARGV)
    MyGameEngine_EnableComponent(${arg})
  endforeach()
endfunction()

# By default enable 2 most fundamental components: engine itself and application entry point
MyGameEngine_EnableComponents(
  Engine
  Launch
)
