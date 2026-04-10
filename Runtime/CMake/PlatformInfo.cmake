set(_collect_platform_info_source "${CMAKE_CURRENT_LIST_DIR}/../Extras/platform-info.cpp")
set(_collect_platform_info_include_dir "${CMAKE_CURRENT_LIST_DIR}/../Include")

# Sets variables:
#   - TARGET_OS
#   - TARGET_OS_NAME
#   - TARGET_ABI_FAMILY
#   - TARGET_ABI_FAMILY_NAME
#   - TARGET_ARCH
#   - TARGET_ARCH_NAME
function(collect_platform_info)
  try_run(_ _
    SOURCE_FROM_FILE "platform-info.cpp" "${_collect_platform_info_source}"
    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${_collect_platform_info_include_dir}"
    RUN_OUTPUT_VARIABLE PLATFORM_INFO
  )

  list(GET PLATFORM_INFO 0 TARGET_OS)
  list(GET PLATFORM_INFO 1 TARGET_OS_NAME)
  list(GET PLATFORM_INFO 2 TARGET_ABI_FAMILY)
  list(GET PLATFORM_INFO 3 TARGET_ABI_FAMILY_NAME)
  list(GET PLATFORM_INFO 4 TARGET_ARCH)
  list(GET PLATFORM_INFO 5 TARGET_ARCH_NAME)

  message(STATUS "Target OS: ${TARGET_OS_NAME}")
  message(STATUS "Target ABI Family: ${TARGET_ABI_FAMILY_NAME}")
  message(STATUS "Target Architecture: ${TARGET_ARCH_NAME}")

  # Expose variables
  set(TARGET_OS "${TARGET_OS}" PARENT_SCOPE)
  set(TARGET_OS_NAME "${TARGET_OS_NAME}" PARENT_SCOPE)
  set(TARGET_ABI_FAMILY "${TARGET_ABI_FAMILY}" PARENT_SCOPE)
  set(TARGET_ABI_FAMILY_NAME "${TARGET_ABI_FAMILY_NAME}" PARENT_SCOPE)
  set(TARGET_ARCH "${TARGET_ARCH}" PARENT_SCOPE)
  set(TARGET_ARCH_NAME "${TARGET_ARCH_NAME}" PARENT_SCOPE)
endfunction()
