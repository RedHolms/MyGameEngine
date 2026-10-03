set(COMPONENT_PRIVATE_SOURCES
  "Private/Graphics/GraphicsAPIDiscovery.cpp"
  "Private/Win32/Windows/Win32Window.cpp"
  "Private/Win32/Windows/Win32Window.hpp"
  "Private/Win32/Windows/Win32WindowHost.cpp"
  "Private/Win32/Windows/Win32WindowHost.hpp"
)

set(COMPONENT_PUBLIC_SOURCES
  "Public/MyGameEngine/GUI/Windows/Window.hpp"
  "Public/MyGameEngine/GUI/Windows/WindowHost.hpp"
)

set(COMPONENT_PRIVATE_DEPENDENCIES
  Graphics/.DiscoveryInterface
  Win32API
)

set(COMPONENT_PUBLIC_DEPENDENCIES
  Core
  Graphics/Base
)
