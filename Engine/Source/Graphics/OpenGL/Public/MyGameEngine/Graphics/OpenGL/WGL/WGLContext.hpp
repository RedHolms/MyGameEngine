#pragma once

#include <MyGameEngine/Graphics/OpenGL/WGL/WGL.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>

class WGLContext : public OpenGLContext, public WGLCommandsProvider {
  // Nothing. This class just adds WGL commands to the OpenGLContext
};
