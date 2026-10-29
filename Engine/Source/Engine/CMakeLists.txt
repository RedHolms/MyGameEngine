add_dependency_component(Core)

add_library(MyGameEngine STATIC)

target_sources(MyGameEngine PRIVATE
  "Private/Graphics/GraphicsAPI.cpp"
  "Private/Graphics/GraphicsAPIFactory.cpp"
  "Private/Graphics/GraphicsAPIsCatalog.cpp"
  "Private/Graphics/GraphicsContext.cpp"
  "Private/Win32/Assets/AssetsManager.cpp"
  "Private/Win32/Windows/Win32Window.cpp"
  "Private/Win32/Windows/Win32WindowHost.cpp"
)

target_sources(MyGameEngine PUBLIC
  "Public/MyGameEngine/Assets/AssetsManager.hpp"
  "Public/MyGameEngine/Graphics/GraphicsAPI.hpp"
  "Public/MyGameEngine/Graphics/GraphicsContext.hpp"
  "Public/MyGameEngine/GUI/Windows/Window.hpp"
  "Public/MyGameEngine/GUI/Windows/WindowHost.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp"
)

target_include_directories(MyGameEngine
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public"
)

target_compile_features(MyGameEngine PUBLIC cxx_std_26)

target_link_libraries(MyGameEngine PUBLIC
  MyGameEngine::Core
)
