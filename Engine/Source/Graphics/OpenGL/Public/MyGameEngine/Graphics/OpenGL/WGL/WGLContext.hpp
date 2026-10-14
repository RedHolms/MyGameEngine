#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>

class WGLCommandsProvider;

class WGLContext : public OpenGLContext {
public:
  virtual WGLCommandsProvider* GetWGLCommandsProvider() = 0;
};
