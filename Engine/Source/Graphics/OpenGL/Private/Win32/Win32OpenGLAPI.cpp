#include <Private/Win32/Win32OpenGLAPI.hpp>

#include <Private/OpenGLFactory.hpp>
#include <Private/Win32/Win32OpenGLContext.hpp>
#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp>
#include <MyGameEngine/win32api/base.hpp>
#include <GL/gl.h>

std::expected<std::unique_ptr<GraphicsAPI>, String> Win32OpenGLAPI::Create(OpenGLFactory* factory) {
  ::HINSTANCE appInstance = ::GetModuleHandleW(nullptr);

  ::WNDCLASSW wc = {
    .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    .lpfnWndProc = ::DefWindowProcW,
    .hInstance = appInstance,
    .lpszClassName = L"MyGameEngine/DummyOpenGlWndClass"
  };
  ::RegisterClassW(&wc);

  ::HWND dummyWindow = ::CreateWindowExW(0, wc.lpszClassName, L"Dummy", 0, 0, 0, 1, 1, nullhandle, nullhandle,
    appInstance, nullptr);
  ::HDC dummyDc = ::GetDC(dummyWindow);

  ::PIXELFORMATDESCRIPTOR dummyPfd {
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

  int dummyFormat = ::ChoosePixelFormat(dummyDc, &dummyPfd);
  ::SetPixelFormat(dummyDc, dummyFormat, &dummyPfd);

  ::HGLRC dummyContext = ::wglCreateContext(dummyDc);
  ::wglMakeCurrent(dummyDc, dummyContext);

  auto wglCreateContextAttribsARB
    = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(::wglGetProcAddress("wglCreateContextAttribsARB"));
  auto wglChoosePixelFormatARB
    = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(::wglGetProcAddress("wglChoosePixelFormatARB"));

  ::wglMakeCurrent(dummyDc, nullhandle);
  ::wglDeleteContext(dummyContext);
  ::ReleaseDC(dummyWindow, dummyDc);
  ::DestroyWindow(dummyWindow);
  ::UnregisterClassW(L"MyGameEngine/DummyOpenGlWndClass", appInstance);

  return std::unique_ptr<GraphicsAPI> {
    new Win32OpenGLAPI(factory, wglCreateContextAttribsARB, wglChoosePixelFormatARB)
  };
}

Win32OpenGLAPI::Win32OpenGLAPI(OpenGLFactory* factory,
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB,
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB)
  : m_factory(factory),
    wglCreateContextAttribsARB(wglCreateContextAttribsARB),
    wglChoosePixelFormatARB(wglChoosePixelFormatARB)
{}

Win32OpenGLAPI::~Win32OpenGLAPI() {
}

GraphicsAPIInformation const& Win32OpenGLAPI::GetInformation() {
  return m_factory->GetInformation();
}

std::expected<std::unique_ptr<OpenGLContext>, String> Win32OpenGLAPI::CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) {
  auto lockedWindow = targetWindow.lock();
  if (!lockedWindow)
    return std::unexpected { U"Window was destroyed"_s };

  if (auto win32Window = dynamic_cast<Win32Window*>(lockedWindow.get())) {
    auto handle = win32Window->GetWin32Handle();

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

    ::HDC windowDrawContext = ::GetDC(handle);

    // TODO: Make pixel attributes chooseable
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

    int pixelFormat;
    ::UINT numFormats;
    wglChoosePixelFormatARB(windowDrawContext, pixelAttribs, nullptr, 1, &pixelFormat, &numFormats);
    ::SetPixelFormat(windowDrawContext, pixelFormat, &pfd);

    // TODO: Make version chooseable
    const int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 3,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0 // Terminate list
  };

    ::HGLRC context = wglCreateContextAttribsARB(windowDrawContext, nullhandle, contextAttribs);

    return std::make_unique<Win32OpenGLContext>(context, windowDrawContext);
  }
  else {
    return std::unexpected { U"Unknown window implementation"_s };
  }
}

std::expected<void, String> Win32OpenGLAPI::MakeContextCurrent(OpenGLContext* context) {
  if (context == nullptr) {
    ::wglMakeCurrent(nullhandle, nullhandle);
    m_boundContextDc = nullhandle;
    return {}; // success
  }

  if (auto win32Context = dynamic_cast<Win32OpenGLContext*>(context)) {
    ::wglMakeCurrent(win32Context->GetDrawContext(), win32Context->GetGLRC());
    m_boundContextDc = win32Context->GetDrawContext();
    return {}; // success
  }
  else {
    return std::unexpected { U"Passed context is not Win32 OpenGL context"_s };
  }
}

bool Win32OpenGLAPI::HasExtension(char const* extensionId) {
  // TODO
  return false;
}

std::expected<void, String> Win32OpenGLAPI::SwapBuffers() {
  bool success = ::SwapBuffers(m_boundContextDc);

  if (!success)
    return std::unexpected { U"::SwapBuffers() failed"_s };

  return {}; // success
}
