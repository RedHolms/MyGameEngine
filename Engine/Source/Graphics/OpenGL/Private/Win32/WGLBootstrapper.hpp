#pragma once

#include <MyGameEngine/Graphics/OpenGL/WGL/WGL.hpp>
#include <MyGameEngine/String.hpp>
#include <expected>

/**
 * Singleton that handles Gdi32.dll/Opengl32.dll loading and creation of first dummy WGL context. Also, it provides some
 * core functions exported from 2 dlls mentioned before.
 */
class WGLBootstrapper {
public:
  static WGLBootstrapper& GetLazyInstance() {
    static WGLBootstrapper instance;
    return instance;
  }

private:
  WGLBootstrapper();
  ~WGLBootstrapper();

public:
  std::expected<void, String> MakeSureFunctionsLoaded();

  // NOTE: Will crash if no call to MakeSureFunctionsLoaded() succeeded before this
  inline void MakeDummyContextCurrent() {
    (void) wglMakeCurrentPtr(m_dummyDeviceContext, m_dummyOpenGlContext);
  }

  void* GetGdi32Symbol(const char* name);
  void* GetOpengl32Symbol(const char* name);

private:
  std::expected<void, String> MakeSureGdi32Loaded();
  std::expected<void, String> MakeSureOpengl32Loaded();
  std::expected<void, String> MakeSureDummyContextCreated();

public:
  // Functions exported from gdi32.dll:
  ChoosePixelFormat_t ChoosePixelFormatPtr = nullptr;
  SetPixelFormat_t SetPixelFormatPtr = nullptr;
  SwapBuffers_t SwapBuffersPtr = nullptr;

  // Functions exported from Opengl32.dll:
  wglCreateContext_t wglCreateContextPtr = nullptr;
  wglMakeCurrent_t wglMakeCurrentPtr = nullptr;
  wglGetCurrentContext_t wglGetCurrentContextPtr = nullptr;
  wglGetCurrentDC_t wglGetCurrentDCPtr = nullptr;
  wglGetProcAddress_t wglGetProcAddressPtr = nullptr;

private:
  ::HMODULE m_gdi32Module = nullhandle;
  ::HMODULE m_opengl32Module = nullhandle;

  ::ATOM m_dummyWndClassAtom = 0;
  ::HWND m_dummyWindow = nullhandle;
  ::HDC m_dummyDeviceContext = nullhandle;
  ::HGLRC m_dummyOpenGlContext = nullhandle;
};
