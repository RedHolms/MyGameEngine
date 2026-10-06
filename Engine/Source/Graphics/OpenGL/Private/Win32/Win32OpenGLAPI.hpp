#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>

// HDC
#include <MyGameEngine/win32api/base.hpp>

#include <GL/gl.h>
#include <GL/wglext.h>

class OpenGLFactory;

class Win32OpenGLAPI final : public OpenGLAPI {
public:
  static std::expected<std::unique_ptr<GraphicsAPI>, String> Create(
    OpenGLFactory* factory);

private:
  explicit Win32OpenGLAPI(OpenGLFactory* factory,
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB,
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB);

public:
  ~Win32OpenGLAPI() override;

public:
  GraphicsAPIInformation const& GetInformation() override;

  std::expected<std::unique_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) override;
  std::expected<void, String> MakeContextCurrent(OpenGLContext* context) override;

  bool HasExtension(char const* extensionId) override;

  std::expected<void, String> SwapBuffers() override;

private:
  OpenGLFactory* m_factory;

  ::HDC m_boundContextDc = nullhandle;
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
};
