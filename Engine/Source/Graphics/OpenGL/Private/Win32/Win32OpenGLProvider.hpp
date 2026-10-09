#pragma once

#include <MyGameEngine/Internal/Graphics/GraphicsAPIProvider.hpp>

#include <MyGameEngine/Win32Inc.hpp>

class Win32OpenGLProvider final : public GraphicsAPIProvider {
public:
  Win32OpenGLProvider();
  ~Win32OpenGLProvider() override;

public:
  String GetDisplayName() override;

  std::expected<std::unique_ptr<GraphicsAPI>, String> Create() override;

public: // Win32OpenGLProvider-specific methods
  bool IsLoaded();

  // Will always return 'nullptr' if IsLoaded() returns false
  void* GetGlProcedureAddr(const char* name) noexcept;

public:
  // Functions exported from Opengl32.dll:
  decltype(&::wglCreateContext) wglCreateContextPtr = nullptr;
  decltype(&::wglMakeCurrent) wglMakeCurrentPtr = nullptr;
  decltype(&::wglGetCurrentContext) wglGetCurrentContextPtr = nullptr;
  decltype(&::wglGetCurrentDC) wglGetCurrentDCPtr = nullptr;
  decltype(&::wglGetProcAddress) wglGetProcAddressPtr = nullptr;

private:
  ::HMODULE m_opengl32Module = nullhandle;

  ::ATOM m_dummyWndClassAtom = 0;
  ::HWND m_dummyWindow = nullhandle;
  ::HDC m_dummyDeviceContext = nullhandle;
  ::HGLRC m_dummyOpenGlContext = nullhandle;
};
