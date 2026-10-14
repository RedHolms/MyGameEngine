#include <Private/Win32/WGLBootstrapper.hpp>

WGLBootstrapper::WGLBootstrapper() = default;
WGLBootstrapper::~WGLBootstrapper() = default;

std::expected<void, String> WGLBootstrapper::MakeSureFunctionsLoaded() {
  if (auto result = MakeSureGdi32Loaded(); !result.has_value())
    return std::unexpected { std::move(result).error() };

  if (auto result = MakeSureOpengl32Loaded(); !result.has_value())
    return std::unexpected { std::move(result).error() };

  if (auto result = MakeSureDummyContextCreated(); !result.has_value())
    return std::unexpected { std::move(result).error() };

  return {}; // success
}

void* WGLBootstrapper::GetGdi32Symbol(const char* name) {
  return reinterpret_cast<void*>(::GetProcAddress(m_gdi32Module, name));
}

void* WGLBootstrapper::GetOpengl32Symbol(const char* name) {
  return reinterpret_cast<void*>(::GetProcAddress(m_opengl32Module, name));
}

std::expected<void, String> WGLBootstrapper::MakeSureGdi32Loaded() {
  if (m_gdi32Module != nullhandle)
    return {}; // success

  ::HMODULE module = ::LoadLibraryW(L"gdi32.dll");

  if (module == nullhandle)
    return std::unexpected { U"Failed to load gdi32.dll"_s }; // TODO: Format win32 error

  ChoosePixelFormatPtr = reinterpret_cast<ChoosePixelFormat_t>(::GetProcAddress(module, "ChoosePixelFormat"));
  SetPixelFormatPtr = reinterpret_cast<SetPixelFormat_t>(::GetProcAddress(module, "SetPixelFormat"));
  SwapBuffersPtr = reinterpret_cast<SwapBuffers_t>(::GetProcAddress(module, "SwapBuffers"));

  if (ChoosePixelFormatPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure ChoosePixelFormat not found in gdi32.dll"_s };
  }
  if (SetPixelFormatPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure SetPixelFormat not found in gdi32.dll"_s };
  }
  if (SwapBuffersPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure SwapBuffers not found in gdi32.dll"_s };
  }

  m_gdi32Module = module;
  return {}; // success
}

std::expected<void, String> WGLBootstrapper::MakeSureOpengl32Loaded() {
  if (m_opengl32Module != nullhandle)
    return {}; // success

  ::HMODULE module = ::LoadLibraryW(L"Opengl32.dll");

  if (module == nullhandle)
    return std::unexpected { U"Failed to load Opengl32.dll"_s }; // TODO: Format win32 error

  wglCreateContextPtr = reinterpret_cast<wglCreateContext_t>(::GetProcAddress(module, "wglCreateContext"));
  wglMakeCurrentPtr = reinterpret_cast<wglMakeCurrent_t>(::GetProcAddress(module, "wglMakeCurrent"));
  wglGetCurrentContextPtr = reinterpret_cast<wglGetCurrentContext_t>(::GetProcAddress(module, "wglGetCurrentContext"));
  wglGetCurrentDCPtr = reinterpret_cast<wglGetCurrentDC_t>(::GetProcAddress(module, "wglGetCurrentDC"));
  wglGetProcAddressPtr = reinterpret_cast<wglGetProcAddress_t>(::GetProcAddress(module, "wglGetProcAddress"));

  if (wglCreateContextPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure wglCreateContext not found in Opengl32.dll"_s };
  }
  if (wglMakeCurrentPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure wglMakeCurrent not found in Opengl32.dll"_s };
  }
  if (wglGetCurrentContextPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure wglGetCurrentContext not found in Opengl32.dll"_s };
  }
  if (wglGetCurrentDCPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure wglGetCurrentDC not found in Opengl32.dll"_s };
  }
  if (wglGetProcAddressPtr == nullptr) {
    ::FreeLibrary(module);
    return std::unexpected { U"Procedure wglGetProcAddress not found in Opengl32.dll"_s };
  }

  m_opengl32Module = module;
  return {}; // success
}

std::expected<void, String> WGLBootstrapper::MakeSureDummyContextCreated() {
  if (m_dummyOpenGlContext != nullhandle)
    return {}; // success

  ::HINSTANCE appInstance = ::GetModuleHandleW(nullptr);

  ::WNDCLASSW wc = {
    .lpfnWndProc = ::DefWindowProcW,
    .hInstance = appInstance,
    .lpszClassName = L"MyGameEngine/DummyOpenGlWndClass"
  };
  ::ATOM dummyAtom = ::RegisterClassW(&wc);

  if (dummyAtom == 0) {
    return std::unexpected { U"Failed to create dummy window class for dummy OpenGL context"_s }; // TODO: Format win32 error
  }

  ::HWND dummyWindow = ::CreateWindowExW(0, wc.lpszClassName, L"Dummy OpenGL Window", 0, 0, 0, 1, 1,
    nullhandle, nullhandle, appInstance, nullptr);

  if (dummyWindow == nullhandle) {
    ::UnregisterClassW(reinterpret_cast<LPCWSTR>(dummyAtom), appInstance);
    return std::unexpected { U"Failed to create dummy window for dummy OpenGL context"_s }; // TODO: Format win32 error
  }

  ::HDC dummyDc = ::GetDC(dummyWindow);

  if (dummyDc == nullhandle) {
    ::DestroyWindow(dummyWindow);
    ::UnregisterClassW(reinterpret_cast<LPCWSTR>(dummyAtom), appInstance);
    return std::unexpected { U"Failed to get draw context of dummy window for dummy OpenGL context"_s }; // TODO: Format win32 error
  }

  ::PIXELFORMATDESCRIPTOR dummyPfd {
    .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cRedBits = 0, .cRedShift = 0,
    .cGreenBits = 0, .cGreenShift = 0,
    .cBlueBits = 0, .cBlueShift = 0,
    .cAlphaBits = 0, .cAlphaShift = 0,
    .cAccumBits = 0,
    .cAccumRedBits = 0, .cAccumGreenBits = 0, .cAccumBlueBits = 0, .cAccumAlphaBits = 0,
    .cDepthBits = 24,
    .cStencilBits = 8,
    .cAuxBuffers = 0,
    .iLayerType = 0,
    .dwLayerMask = 0, .dwVisibleMask = 0, .dwDamageMask = 0
  };

  int dummyFormat = ChoosePixelFormatPtr(dummyDc, &dummyPfd); // TODO: Check it?
  (void) SetPixelFormatPtr(dummyDc, dummyFormat, &dummyPfd); // TODO: Check it?

  ::HGLRC dummyContext = wglCreateContextPtr(dummyDc);

  if (dummyContext == nullhandle) {
    ::DestroyWindow(dummyWindow);
    ::UnregisterClassW(reinterpret_cast<LPCWSTR>(dummyAtom), appInstance);
    return std::unexpected { U"Failed to create dummy OpenGL context"_s }; // TODO: Format win32 error
  }

  m_dummyWndClassAtom = dummyAtom;
  m_dummyWindow = dummyWindow;
  m_dummyDeviceContext = dummyDc;
  m_dummyOpenGlContext = dummyContext;

  return {}; // success
}
