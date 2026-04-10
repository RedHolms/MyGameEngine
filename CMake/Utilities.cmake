function(glob_target_sources TARGET)
  if(NOT TARGET ${TARGET})
    message(FATAL_ERROR "Target '${TARGET}' does not exist")
  endif()

  set(current_scope "")
  set(scopes PUBLIC PRIVATE INTERFACE)

  foreach(arg IN LISTS ARGN)
    list(FIND scopes "${arg}" scope_index)

    if(NOT scope_index EQUAL -1)
      set(current_scope "${arg}")
      continue()
    endif()

    if(NOT current_scope)
      message(FATAL_ERROR "glob_target_sources: Missing scope (PUBLIC/PRIVATE/INTERFACE) before '${arg}'")
    endif()

    if(IS_ABSOLUTE "${arg}")
      set(pattern "${arg}")
    else()
      set(pattern "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
    endif()

    file(GLOB_RECURSE files CONFIGURE_DEPENDS "${pattern}")

    if(files)
      target_sources(${TARGET} ${current_scope} ${files})
    endif()
  endforeach()
endfunction()
