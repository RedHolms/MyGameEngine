#include <Private/Win32/Win32OpenGLProvider.hpp>

#include <Private/Win32/Win32OpenGLAPI.hpp>

Win32OpenGLProvider::Win32OpenGLProvider() = default;
Win32OpenGLProvider::~Win32OpenGLProvider() = default;

String Win32OpenGLProvider::GetDisplayName() {
  return U"OpenGL"_s;
}

std::expected<std::unique_ptr<GraphicsAPI>, String> Win32OpenGLProvider::Create() {
  if (m_opengl32Module == nullhandle) {
    ::HMODULE glModule = ::LoadLibraryW(L"Opengl32.dll");

    if (glModule == nullhandle)
      return std::unexpected { U"Failed to load Opengl32.dll"_s }; // TODO: Format win32 error

    wglCreateContextPtr = reinterpret_cast<decltype(&::wglCreateContext)>(::GetProcAddress(glModule, "wglCreateContext"));
    wglMakeCurrentPtr = reinterpret_cast<decltype(&::wglMakeCurrent)>(::GetProcAddress(glModule, "wglMakeCurrent"));
    wglGetCurrentContextPtr = reinterpret_cast<decltype(&::wglGetCurrentContext)>(::GetProcAddress(glModule, "wglGetCurrentContext"));
    wglGetCurrentDCPtr = reinterpret_cast<decltype(&::wglGetCurrentDC)>(::GetProcAddress(glModule, "wglGetCurrentDC"));
    wglGetProcAddressPtr = reinterpret_cast<decltype(&::wglGetProcAddress)>(::GetProcAddress(glModule, "wglGetProcAddress"));

    if (wglCreateContextPtr == nullptr) {
      ::FreeLibrary(glModule);
      return std::unexpected { U"Procedure wglCreateContext not found in Opengl32.dll"_s };
    }
    if (wglMakeCurrentPtr == nullptr) {
      ::FreeLibrary(glModule);
      return std::unexpected { U"Procedure wglMakeCurrent not found in Opengl32.dll"_s };
    }
    if (wglGetCurrentContextPtr == nullptr) {
      ::FreeLibrary(glModule);
      return std::unexpected { U"Procedure wglGetCurrentContext not found in Opengl32.dll"_s };
    }
    if (wglGetCurrentDCPtr == nullptr) {
      ::FreeLibrary(glModule);
      return std::unexpected { U"Procedure wglGetCurrentDC not found in Opengl32.dll"_s };
    }
    if (wglGetProcAddressPtr == nullptr) {
      ::FreeLibrary(glModule);
      return std::unexpected { U"Procedure wglGetProcAddress not found in Opengl32.dll"_s };
    }

    m_opengl32Module = glModule;
  }

  if (m_dummyOpenGlContext == nullhandle) {
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

    int dummyFormat = ::ChoosePixelFormat(dummyDc, &dummyPfd); // TODO: Check it?
    (void) ::SetPixelFormat(dummyDc, dummyFormat, &dummyPfd); // TODO: Check it?

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
  }

  return std::make_unique<Win32OpenGLAPI>(this);
}

bool Win32OpenGLProvider::IsLoaded() {
  return m_dummyOpenGlContext != nullhandle;
}

void* Win32OpenGLProvider::GetGlProcedureAddr(const char* name) noexcept {
  if (m_dummyOpenGlContext == nullhandle)
    return nullptr;

  auto prevCtx = wglGetCurrentContextPtr();
  auto prevDc = wglGetCurrentDCPtr();

  wglMakeCurrentPtr(m_dummyDeviceContext, m_dummyOpenGlContext);
  auto result = wglGetProcAddressPtr(name);
  wglMakeCurrentPtr(prevDc, prevCtx);

  return reinterpret_cast<void*>(result);
}
