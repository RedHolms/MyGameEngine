set(COMPONENT_PRIVATE_SOURCES
  "Private/Win32/OpenGLRegistrar.cpp"
  "Private/Win32/Win32OpenGLAPI.cpp"
  "Private/Win32/Win32OpenGLAPI.hpp"
  "Private/Win32/Win32OpenGLContext.cpp"
  "Private/Win32/Win32OpenGLContext.hpp"
  "Private/Win32/Win32OpenGLProvider.cpp"
  "Private/Win32/Win32OpenGLProvider.hpp"
  "Private/OpenGLContext.cpp"
)

set(COMPONENT_PUBLIC_SOURCES
  "Public/GL/glext.h"
  "Public/GL/wglext.h"
  "Public/KHR/khrplatform.h"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp"
)

set(COMPONENT_PRIVATE_DEPENDENCIES
  Engine
  Graphics/Base
  Win32API
)

set(COMPONENT_REQUIRE_FULL_LINKAGE TRUE)
