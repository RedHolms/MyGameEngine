MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  DEPENDS_ON
    Core
)

target_sources(${TARGET} PRIVATE
  "Private/GraphicsAPI.cpp"
  "Private/GraphicsAPIProvider.cpp"
  "Private/GraphicsAPIsCatalog.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/MyGameEngine/Graphics/GraphicsAPI.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIProvider.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp"
)
