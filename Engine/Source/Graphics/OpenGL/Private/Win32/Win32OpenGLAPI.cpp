#include <Private/Win32/Win32OpenGLAPI.hpp>

#include <Private/Win32/WGLBootstrapper.hpp>
#include <Private/Win32/Win32OpenGLContext.hpp>
#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp>
#include <MyGameEngine/Win32Inc.hpp>

Win32OpenGLAPI::Win32OpenGLAPI() = default;
Win32OpenGLAPI::~Win32OpenGLAPI() = default;

std::expected<std::shared_ptr<OpenGLContext>, String> Win32OpenGLAPI::CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();

  // NOTE: Probably it's redundant here because Win32OpenGLFactory does the same thing before even creating us, but
  // let's be safe
  if (auto result = bootstrapper.MakeSureFunctionsLoaded(); !result.has_value())
    return std::unexpected { std::move(result).error() };

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

  bootstrapper.MakeDummyContextCurrent();

  // TODO: Check for nullptrs
  auto wglChoosePixelFormatARB =
    reinterpret_cast<wglChoosePixelFormatARB_t>(bootstrapper.wglGetProcAddressPtr("wglChoosePixelFormatARB"));
  auto wglCreateContextAttribsARB =
    reinterpret_cast<wglCreateContextAttribsARB_t>(bootstrapper.wglGetProcAddressPtr("wglCreateContextAttribsARB"));

  int pixelFormat;
  ::UINT numFormats;
  wglChoosePixelFormatARB(deviceContext, pixelAttribs, nullptr, 1, &pixelFormat, &numFormats);
  bootstrapper.SetPixelFormatPtr(deviceContext, pixelFormat, &pfd);

  // TODO: Add ability to change these properties
  const int contextAttribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0 // Terminate list
};

  ::HGLRC context = wglCreateContextAttribsARB(deviceContext, nullhandle, contextAttribs);

  auto instance = std::make_shared<Win32OpenGLContext>(context, deviceContext);
  return std::shared_ptr<OpenGLContext> { std::move(instance), static_cast<OpenGLContext*>(instance.get()) };
}
