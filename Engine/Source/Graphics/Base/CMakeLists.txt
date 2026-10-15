MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  DEPENDS_ON
    Core
)

target_sources(${TARGET} PRIVATE
  "Private/GraphicsAPI.cpp"
  "Private/GraphicsAPIFactory.cpp"
  "Private/GraphicsAPIsCatalog.cpp"
  "Private/GraphicsContext.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/MyGameEngine/Graphics/GraphicsAPI.hpp"
  "Public/MyGameEngine/Graphics/GraphicsContext.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp"
  "Public/MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp"
)
