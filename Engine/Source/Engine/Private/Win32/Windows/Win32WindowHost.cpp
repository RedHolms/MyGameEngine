#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp>

#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp>
#include <MyGameEngine/EncodedString.hpp>

#include <MyGameEngine/win32api/base.hpp>

namespace {

struct Win32WindowCreationParameters {
  Win32WindowHost* host;
};

} // namespace <anonymous>

Win32WindowHost::Win32WindowHost(::HINSTANCE classOwnerInstance, String const& className, ::ATOM classAtom)
  : m_classOwnerInstance(classOwnerInstance),
    m_className(className),
    m_classAtom(classAtom)
{
  m_windowsThreadWakeUpEvent = ::CreateEventW(nullptr, false, false, nullptr);
  m_windowsThread = std::thread { &Win32WindowHost::WindowsThreadBody, this };
}

Win32WindowHost::~Win32WindowHost() {
  // TODO: Destroy all owned windows and destroy window class
}

std::expected<std::shared_ptr<Window>, String> Win32WindowHost::CreateWindow(String const& title, uint width, uint height) {
  Win32WindowCreationRequest request {
    .title = title,
    .width = width,
    .height = height
  };

  auto future = request.promise.get_future();

  {
    // Push request to the queue
    std::lock_guard windowCreationRequestQueueGuard { m_windowCreationRequestQueueMutex };
    m_windowCreationRequestQueue.emplace_back(std::move(request));
  }

  // Ask windows thread to wake up
  ::SetEvent(m_windowsThreadWakeUpEvent);

  // Wait for the request to complete
  return future.get();
}

void Win32WindowHost::WindowsThreadBody() {
  ::HANDLE handlesToWait[] = { m_windowsThreadWakeUpEvent };

  while (!m_windowsThreadExitFlag.test()) {
    WindowsThread_ProcessWindowCreationRequests();
    WindowsThread_ProcessWindowMessages();

    // Sleep until someone notifies us or new message arrives
    (void) ::MsgWaitForMultipleObjectsEx(
      std::size(handlesToWait), handlesToWait,
      INFINITE,
      QS_ALLINPUT,
      MWMO_INPUTAVAILABLE);
  }
}

void Win32WindowHost::WindowsThread_ProcessWindowCreationRequests() {
  Win32WindowCreationRequest request {};

  while (true) {
    {
      std::lock_guard windowCreationRequestQueueGuard { m_windowCreationRequestQueueMutex };
      if (m_windowCreationRequestQueue.empty())
        return;

      request = std::move(m_windowCreationRequestQueue.front());
      m_windowCreationRequestQueue.pop_front();
    }

    WindowsThread_ProcessWindowCreationRequest(request);
  }
}

void Win32WindowHost::WindowsThread_ProcessWindowCreationRequest(Win32WindowCreationRequest& request) {
  auto titleWChar = request.title.Encode(Encoding::WCHAR);

  Win32WindowCreationParameters parameters {
    .host = this
  };

  auto windowHandle = ::CreateWindowExW(
    0,
    reinterpret_cast<const wchar_t*>(m_classAtom),
    titleWChar.GetCString<wchar_t>(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    request.width,
    request.height,
    nullhandle,
    nullhandle,
    m_classOwnerInstance,
    &parameters);

  if (windowHandle == nullhandle) {
    // TODO: Format win32 error message
    request.promise.set_value(std::unexpected { U"Failed to create a window"_s });
    return;
  }

  ::ShowWindow(windowHandle, SW_SHOW);
  ::UpdateWindow(windowHandle);

  auto windowInstance = std::make_shared<Win32Window>(this, windowHandle);
  request.promise.set_value(std::move(windowInstance));
}

void Win32WindowHost::WindowsThread_ProcessWindowMessages() {
  ::MSG msg {};
  while (::PeekMessageW(&msg, nullhandle, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);
  }
}

::LRESULT Win32WindowHost::StaticWndProc(::HWND window, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam) {
  switch (msg) {

  }


  // Pass message to Win32
  return ::DefWindowProcW(window, msg, wparam, lparam);
}

std::expected<std::shared_ptr<WindowHost>, String> WindowHost::Create(String const& uniqueName) {
  auto classOwnerInstance = ::GetModuleHandleW(nullptr);

  // Make a name for the class
  auto className = U"MyGameEngineWindowClass/"_s + uniqueName;
  auto classNameWChar = className.Encode(Encoding::WCHAR);

  ::WNDCLASSEXW wndclassexw {
    .cbSize = sizeof(::WNDCLASSEXW),
    .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    .lpfnWndProc = &Win32WindowHost::StaticWndProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = classOwnerInstance,
    .hIcon = nullhandle,
    .hCursor = nullhandle,
    .hbrBackground = nullhandle,
    .lpszMenuName = nullptr,
    .lpszClassName = classNameWChar.GetCString<wchar_t>(),
    .hIconSm = nullhandle
  };

  // Try to register window class
  auto atom = ::RegisterClassExW(&wndclassexw);
  if (atom == 0) {
    // Registration failed. TODO: Return formatted win32 error info
    return std::unexpected {U"Failed to create window class"_s};
  }

  // Create an instance of window host implementation
  auto hostInstance = std::make_shared<Win32WindowHost>(classOwnerInstance, className, atom);
  return hostInstance;
}
