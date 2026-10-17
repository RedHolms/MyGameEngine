add_dependency_component(Core)

add_library(MyGameEngine_Launch STATIC)
add_library(MyGameEngine::Launch ALIAS MyGameEngine_Launch)

target_sources(MyGameEngine_Launch PRIVATE
  "Private/WinMain.cpp"
)

target_include_directories(MyGameEngine_Launch
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(MyGameEngine_Launch PUBLIC cxx_std_26)

target_link_libraries(MyGameEngine_Launch PUBLIC
  MyGameEngine::Core
)
