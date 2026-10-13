MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  REQUIRE_FULL_LINKAGE
    TRUE
  DEPENDS_ON
    Engine
    Graphics/Base
)

target_sources(${TARGET} PRIVATE
  "Private/Win32/OpenGLRegistrar.cpp"
  "Private/Win32/Win32OpenGLAPI.cpp"
  "Private/Win32/Win32OpenGLAPI.hpp"
  "Private/Win32/Win32OpenGLContext.cpp"
  "Private/Win32/Win32OpenGLContext.hpp"
  "Private/Win32/Win32OpenGLProvider.cpp"
  "Private/Win32/Win32OpenGLProvider.hpp"
  "Private/OpenGLContext.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/GL/glext.h"
  "Public/GL/wglext.h"
  "Public/KHR/khrplatform.h"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp"
)
