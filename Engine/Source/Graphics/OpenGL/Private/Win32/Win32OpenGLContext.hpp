#pragma once

#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>
#include <memory>

#include <MyGameEngine/win32api/base.hpp>

class Win32OpenGLContext final : public OpenGLContext {
public:
  Win32OpenGLContext(::HGLRC glRc, ::HDC dc);
  ~Win32OpenGLContext() override;

public:
  OpenGLContextTarget GetTarget() override;

public: // Win32OpenGLContext-specific methods
  ::HGLRC GetGLRC() const noexcept {
    return m_glResource;
  }

  ::HDC GetDrawContext() const noexcept {
    return m_drawContext;
  }

private:
  ::HGLRC m_glResource;
  ::HDC m_drawContext;
};
