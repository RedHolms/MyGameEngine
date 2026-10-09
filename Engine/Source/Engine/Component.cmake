set(COMPONENT_PRIVATE_SOURCES
  "Private/Win32/Windows/Win32Window.cpp"
  "Private/Win32/Windows/Win32WindowHost.cpp"
)

set(COMPONENT_PUBLIC_SOURCES
  "Public/MyGameEngine/GUI/Windows/Window.hpp"
  "Public/MyGameEngine/GUI/Windows/WindowHost.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp"
  "Public/MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp"
)

set(COMPONENT_PUBLIC_DEPENDENCIES
  Core
  Graphics/Base
)
