MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  DEPENDS_ON
    Core
    Graphics/Base
)

target_sources(${TARGET} PRIVATE
  "Private/Win32/Windows/Win32Window.cpp"
  "Private/Win32/Windows/Win32WindowHost.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/MyGameEngine/GUI/Windows/Window.hpp"
  "Public/MyGameEngine/GUI/Windows/WindowHost.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp"
)
