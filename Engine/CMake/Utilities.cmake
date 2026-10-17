function(_mge_touch_generated_directory DIRECTORY_PATH)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  add_custom_command(
    OUTPUT
      "${DIRECTORY_PATH}/.gitignore"
    COMMAND
      ${Python3_EXECUTABLE} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../Build/make-generated-directory.py" "${DIRECTORY_PATH}"
  )
endfunction()
