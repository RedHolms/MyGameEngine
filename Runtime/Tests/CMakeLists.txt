enable_testing()

function(add_test_target NAME)
  add_executable(${NAME} "main.cpp" "Tests/${NAME}.cpp")
  target_link_libraries(${NAME} PRIVATE MyGameEngineRuntime)
  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

add_test_target(ListTests)
