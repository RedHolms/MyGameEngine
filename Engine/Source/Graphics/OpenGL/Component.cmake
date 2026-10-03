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
  "Public/KHR/khrplatform.h"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGLAPI.hpp"
)

set(COMPONENT_PRIVATE_DEPENDENCIES
  Graphics/.DiscoveryInterface
  Graphics/Base
)

set(COMPONENT_PUBLIC_DEPENDENCIES
  # FIXME Really bad, win32 should not be a public dependency but required for WGL.hpp
  Win32API
)

set(COMPONENT_REQUIRE_FULL_LINKAGE TRUE)
