set(COMPONENT_PRIVATE_SOURCES
  "Private/GraphicsAPI.cpp"
  "Private/GraphicsAPIProvider.cpp"
  "Private/GraphicsAPIsCatalog.cpp"
)

set(COMPONENT_PUBLIC_SOURCES
  "Public/MyGameEngine/Graphics/GraphicsAPI.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIProvider.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp"
)

set(COMPONENT_PUBLIC_DEPENDENCIES
  Core
)
