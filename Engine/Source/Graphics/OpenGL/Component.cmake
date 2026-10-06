set(COMPONENT_PRIVATE_SOURCES
  "Private/Win32/OpenGLFactory.cpp"
  "Private/Win32/Win32OpenGLAPI.cpp"
  "Private/Win32/Win32OpenGLAPI.hpp"
  "Private/Win32/Win32OpenGLContext.cpp"
  "Private/Win32/Win32OpenGLContext.hpp"
  "Private/OpenGLFactory.hpp"
  "Private/OpenGLRegistrar.cpp"
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
  Graphics/.DiscoveryInterface
  Graphics/Base
)

set(COMPONENT_PUBLIC_DEPENDENCIES
  # FIXME Really bad, win32 should not be a public dependency but required for WGL.hpp
  Win32API
)

set(COMPONENT_EXTERNAL_LINK_LIBRARIES
  Opengl32.lib
)

set(COMPONENT_REQUIRE_FULL_LINKAGE TRUE)
