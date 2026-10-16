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

#
# MyGameEngine_AddAsset(TARGET <game> ARCHIVE <name> FILE <path> [AS <archive-relative-path>] [GENERATED_BY <target>])
#
# Adds one file to a named archive packaged beside the game executable. Calls may be repeated to add assets or
# archives. FILE may name a generated file, provided its producing custom command/target is part of the build graph.
#
function(MyGameEngine_AddAsset)
  cmake_parse_arguments(PARSE_ARGV 0 ARG "" "TARGET;ARCHIVE;FILE;AS;GENERATED_BY" "")

  if(ARG_UNPARSED_ARGUMENTS OR ARG_KEYWORDS_MISSING_VALUES OR NOT ARG_TARGET OR NOT ARG_ARCHIVE OR NOT ARG_FILE)
    message(FATAL_ERROR "Usage: MyGameEngine_AddAsset(TARGET <game> ARCHIVE <name> FILE <path> [AS <archive-relative-path>] [GENERATED_BY <target>])")
  endif()
  if(NOT TARGET "${ARG_TARGET}")
    message(FATAL_ERROR "MyGameEngine_AddAsset: target '${ARG_TARGET}' does not exist")
  endif()
  if(NOT ARG_ARCHIVE MATCHES "^[A-Za-z0-9_.-]+$")
    message(FATAL_ERROR "MyGameEngine_AddAsset: ARCHIVE must be a filename stem, got '${ARG_ARCHIVE}'")
  endif()

  if(IS_ABSOLUTE "${ARG_FILE}")
    cmake_path(NORMAL_PATH ARG_FILE OUTPUT_VARIABLE asset_source)
  else()
    cmake_path(ABSOLUTE_PATH ARG_FILE BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE asset_source)
  endif()

  if(ARG_AS)
    set(asset_name "${ARG_AS}")
  else()
    get_filename_component(asset_name "${ARG_FILE}" NAME)
  endif()
  string(REPLACE "\\" "/" asset_name "${asset_name}")
  if(asset_name STREQUAL "" OR asset_name MATCHES ";" OR IS_ABSOLUTE "${asset_name}" OR
     asset_name MATCHES "//" OR asset_name MATCHES "/$" OR asset_name MATCHES ":")
    message(FATAL_ERROR "MyGameEngine_AddAsset: AS must be a normalized archive-relative path, got '${asset_name}'")
  endif()
  string(REPLACE "/" ";" asset_name_parts "${asset_name}")
  if("." IN_LIST asset_name_parts OR ".." IN_LIST asset_name_parts)
    message(FATAL_ERROR "MyGameEngine_AddAsset: AS cannot contain '.' or '..' path segments, got '${asset_name}'")
  endif()
  if(ARG_GENERATED_BY AND NOT TARGET "${ARG_GENERATED_BY}")
    message(FATAL_ERROR "MyGameEngine_AddAsset: GENERATED_BY target '${ARG_GENERATED_BY}' does not exist")
  endif()
  if(NOT ARG_GENERATED_BY)
    set(ARG_GENERATED_BY "__MYGAMEENGINE_NO_ASSET_PRODUCER__")
  endif()

  get_property(asset_records GLOBAL PROPERTY "MYGAMEENGINE_ASSETS_${ARG_TARGET}_${ARG_ARCHIVE}")
  list(APPEND asset_records "${asset_source}" "${asset_name}" "${ARG_GENERATED_BY}")
  set_property(GLOBAL PROPERTY "MYGAMEENGINE_ASSETS_${ARG_TARGET}_${ARG_ARCHIVE}" "${asset_records}")

  get_property(archives GLOBAL PROPERTY "MYGAMEENGINE_ASSET_ARCHIVES_${ARG_TARGET}")
  if(NOT ARG_ARCHIVE IN_LIST archives)
    list(APPEND archives "${ARG_ARCHIVE}")
    set_property(GLOBAL PROPERTY "MYGAMEENGINE_ASSET_ARCHIVES_${ARG_TARGET}" "${archives}")
  endif()

  get_property(game_targets GLOBAL PROPERTY MYGAMEENGINE_ASSET_GAME_TARGETS)
  if(NOT ARG_TARGET IN_LIST game_targets)
    list(APPEND game_targets "${ARG_TARGET}")
    set_property(GLOBAL PROPERTY MYGAMEENGINE_ASSET_GAME_TARGETS "${game_targets}")
    get_target_property(target_binary_dir "${ARG_TARGET}" BINARY_DIR)
    set_property(GLOBAL PROPERTY "MYGAMEENGINE_ASSET_OUTPUT_DIR_${ARG_TARGET}" "${target_binary_dir}")
  endif()
  get_property(scheduled_roots GLOBAL PROPERTY MYGAMEENGINE_ASSET_FINALIZER_SCHEDULED_ROOTS)
  if(NOT CMAKE_SOURCE_DIR IN_LIST scheduled_roots)
    list(APPEND scheduled_roots "${CMAKE_SOURCE_DIR}")
    set_property(GLOBAL PROPERTY MYGAMEENGINE_ASSET_FINALIZER_SCHEDULED_ROOTS "${scheduled_roots}")
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL __my_game_engine_finalize_assets)
  endif()
endfunction()

function(__my_game_engine_finalize_assets)
  get_property(game_targets GLOBAL PROPERTY MYGAMEENGINE_ASSET_GAME_TARGETS)
  get_property(finalized_targets GLOBAL PROPERTY MYGAMEENGINE_ASSET_FINALIZED_TARGETS)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  foreach(TARGET_NAME IN LISTS game_targets)
    if(TARGET_NAME IN_LIST finalized_targets)
      continue()
    endif()
    get_property(OUTPUT_BASE_DIR GLOBAL PROPERTY "MYGAMEENGINE_ASSET_OUTPUT_DIR_${TARGET_NAME}")
    get_property(archives GLOBAL PROPERTY "MYGAMEENGINE_ASSET_ARCHIVES_${TARGET_NAME}")
    foreach(archive IN LISTS archives)
      get_property(asset_records GLOBAL PROPERTY "MYGAMEENGINE_ASSETS_${TARGET_NAME}_${archive}")
      set(output "${OUTPUT_BASE_DIR}/MyGameEngineAssets/$<CONFIG>/${archive}.mga")
      set(asset_args "")
      set(asset_dependencies "")
      set(producer_targets "")
      while(asset_records)
        list(POP_FRONT asset_records source_path)
        list(POP_FRONT asset_records archive_path)
        list(POP_FRONT asset_records producer_target)
        list(APPEND asset_args --asset "${source_path}" "${archive_path}")
        list(APPEND asset_dependencies "${source_path}")
        if(NOT producer_target STREQUAL "__MYGAMEENGINE_NO_ASSET_PRODUCER__")
          list(APPEND producer_targets "${producer_target}")
        endif()
      endwhile()

      add_custom_command(
        OUTPUT "${output}"
        COMMAND "${Python3_EXECUTABLE}" "${MYGAMEENGINE_SOURCE_DIR}/Build/pack-assets.py"
          --output "${output}" ${asset_args}
        DEPENDS "${MYGAMEENGINE_SOURCE_DIR}/Build/pack-assets.py" ${asset_dependencies}
        COMMENT "Packing ${archive}.mga for ${TARGET_NAME}"
        VERBATIM
      )
      set(asset_target "${TARGET_NAME}_assets_${archive}")
      add_custom_target("${asset_target}" DEPENDS "${output}")
      foreach(producer_target IN LISTS producer_targets)
        add_dependencies("${asset_target}" "${producer_target}")
      endforeach()

      # This always-run copy target handles generators where order-only dependencies do not rerun link post-build
      # events when an asset changes. The output directory is created before the executable is linked.
      set(copy_target "${TARGET_NAME}_copy_assets_${archive}")
      add_custom_target("${copy_target}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${output}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/${archive}.mga"
        COMMENT "Copying ${archive}.mga beside ${TARGET_NAME}"
        VERBATIM
      )
      add_dependencies("${copy_target}" "${asset_target}")
      add_dependencies("${TARGET_NAME}" "${copy_target}")
    endforeach()
    list(APPEND finalized_targets "${TARGET_NAME}")
    set_property(GLOBAL PROPERTY MYGAMEENGINE_ASSET_FINALIZED_TARGETS "${finalized_targets}")
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
