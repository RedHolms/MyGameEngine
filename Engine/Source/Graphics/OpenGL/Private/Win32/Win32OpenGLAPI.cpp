#include <Private/Win32/Win32OpenGLAPI.hpp>

#include <Private/Win32/Win32OpenGLContext.hpp>
#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp>
#include <MyGameEngine/Win32Inc.hpp>
#include <GL/GL.h>
#include <GL/wglext.h>

Win32OpenGLAPI::Win32OpenGLAPI(Win32OpenGLProvider* provider)
  : m_provider(provider)
{}

Win32OpenGLAPI::~Win32OpenGLAPI() {
  // TODO?
}

std::expected<std::shared_ptr<OpenGLContext>, String> Win32OpenGLAPI::CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) {
  auto lockedWindow = targetWindow.lock();

  if (!lockedWindow)
    return std::unexpected { U"Window was destroyed"_s };

  auto win32Window = dynamic_cast<Win32Window*>(lockedWindow.get());
  if (!win32Window)
    return std::unexpected { U"Given window is not Win32 window"_s };

  // TODO: Add ability to change these properties
  ::PIXELFORMATDESCRIPTOR pfd {
    .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cRedBits = 0, .cRedShift = 0,
    .cGreenBits = 0, .cGreenShift = 0,
    .cBlueBits = 0, .cBlueShift = 0,
    .cAlphaBits = 0, .cAlphaShift = 0,
    .cAccumBits = 0,
    .cAccumRedBits = 0,
    .cAccumGreenBits = 0,
    .cAccumBlueBits = 0,
    .cAccumAlphaBits = 0,
    .cDepthBits = 24,
    .cStencilBits = 8,
    .cAuxBuffers = 0,
    .iLayerType = 0,
    .dwLayerMask = 0,
    .dwVisibleMask = 0,
    .dwDamageMask = 0
  };

  ::HDC deviceContext = ::GetDC(win32Window->GetWin32Handle());

  // TODO: Add ability to change these properties
  const int pixelAttribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0 // Terminate list
  };

  // TODO: Don't get procedures like that
  // TODO: Check for nullptrs
  auto wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(m_provider->GetGlProcedureAddr("wglChoosePixelFormatARB"));
  auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(m_provider->GetGlProcedureAddr("wglCreateContextAttribsARB"));

  int pixelFormat;
  ::UINT numFormats;
  wglChoosePixelFormatARB(deviceContext, pixelAttribs, nullptr, 1, &pixelFormat, &numFormats);
  ::SetPixelFormat(deviceContext, pixelFormat, &pfd);

  // TODO: Add ability to change these properties
  const int contextAttribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0 // Terminate list
};

  ::HGLRC context = wglCreateContextAttribsARB(deviceContext, nullhandle, contextAttribs);

  return std::make_unique<Win32OpenGLContext>(context, deviceContext);
}

std::expected<void, String> Win32OpenGLAPI::MakeContextCurrent(std::shared_ptr<OpenGLContext> const& contextSharedPtr) {
  if (contextSharedPtr == nullptr) {
    m_provider->wglMakeCurrentPtr(nullptr, nullptr);
    m_boundContext.reset();
    return {}; // success
  }

  if (auto win32Context = dynamic_cast<Win32OpenGLContext*>(contextSharedPtr.get())) {
    bool success = m_provider->wglMakeCurrentPtr(win32Context->GetDeviceContext(), win32Context->GetGLRC());
    if (!success)
      return std::unexpected { U"wglMakeCurrent() failed"_s }; // TODO: Format win32 error

    m_boundContext = std::shared_ptr<Win32OpenGLContext> { contextSharedPtr, win32Context };

    return {}; // success
  }
  else {
    return std::unexpected { U"Given context is not Win32 OpenGL context"_s };
  }
}

bool Win32OpenGLAPI::HasExtension(char const* extensionId) {
  // TODO
  (void) extensionId;
  return false;
}

std::expected<void, String> Win32OpenGLAPI::SwapBuffers() {
  if (m_boundContext == nullptr)
    return std::unexpected { U"No context bound"_s };

  bool success = ::SwapBuffers(m_boundContext->GetDeviceContext());

  if (!success)
    return std::unexpected { U"::SwapBuffers() failed"_s }; // TODO: Format win32 error

  return {}; // success
}
