#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>

// HGLRC, HDC
#include <MyGameEngine/win32api/base.hpp>

class Win32OpenGLContext final : public OpenGLContext {
public:
  Win32OpenGLContext(::HGLRC glRc, ::HDC dc);
  ~Win32OpenGLContext() override;

public: // Win32OpenGLContext-specific methods
  ::HGLRC GetGLRC() const noexcept {
    return m_glResource;
  }

  ::HDC GetDeviceContext() const noexcept {
    return m_deviceContext;
  }

private:
  ::HGLRC m_glResource;
  ::HDC m_deviceContext;
};
