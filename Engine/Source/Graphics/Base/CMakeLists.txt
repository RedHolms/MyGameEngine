add_dependency_component(Core)

add_library(MyGameEngine_Graphics_Base STATIC)
add_library(MyGameEngine::Graphics::Base ALIAS MyGameEngine_Graphics_Base)

target_sources(MyGameEngine_Graphics_Base PRIVATE
  "Private/GraphicsAPI.cpp"
  "Private/GraphicsAPIFactory.cpp"
  "Private/GraphicsAPIsCatalog.cpp"
  "Private/GraphicsContext.cpp"
)

target_sources(MyGameEngine_Graphics_Base PUBLIC
  "Public/MyGameEngine/Graphics/GraphicsAPI.hpp"
  "Public/MyGameEngine/Graphics/GraphicsContext.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp"
)

target_include_directories(MyGameEngine_Graphics_Base
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public"
)

target_compile_features(MyGameEngine_Graphics_Base PUBLIC cxx_std_26)

target_link_libraries(MyGameEngine_Graphics_Base PUBLIC
  MyGameEngine::Core
)
