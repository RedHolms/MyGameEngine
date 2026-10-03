#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>

class Win32OpenGLContext final : public OpenGLContext {
public:
  ~Win32OpenGLContext() override;

public:
  OpenGLContextTarget GetTarget() override;



private:

};
