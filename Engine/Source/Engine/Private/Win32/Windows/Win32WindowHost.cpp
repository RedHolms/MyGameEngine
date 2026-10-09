#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp>

#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32Window.hpp>
#include <MyGameEngine/EncodedString.hpp>
#include <MyGameEngine/Win32Inc.hpp>

namespace {

struct Win32WindowCreationParameters {
  Win32WindowHost* host;
  std::shared_ptr<Win32Window> outInstance;
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

  // Set exit flag
  (void) m_windowsThreadExitFlag.test_and_set();

  while (m_windowsThread.joinable()) {
    // Wait until thread is dead
    (void) ::SetEvent(m_windowsThreadWakeUpEvent);
    m_windowsThread.join();
  }
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

  request.promise.set_value(std::move(parameters.outInstance));
}

void Win32WindowHost::WindowsThread_ProcessWindowMessages() {
  ::MSG msg {};
  while (::PeekMessageW(&msg, nullhandle, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessageW(&msg);
  }
}

std::shared_ptr<Win32Window> Win32WindowHost::ProcessWindowCreation(::HWND handle) {
  auto instance = std::make_shared<Win32Window>(this, handle);

  // SAFE because Win32Window's destructor destroys the window and this pointer becomes inaccessible.
  ::SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance.get()));

  return std::move(instance);
}

::LRESULT Win32WindowHost::StaticWndProc(::HWND window, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam) {
  if (msg == WM_CREATE) {
    // Process window creation. lpCreateParams must be a pointer to Win32WindowCreationParameters

    auto createStruct = reinterpret_cast<LPCREATESTRUCTW>(lparam);
    auto params = static_cast<Win32WindowCreationParameters*>(createStruct->lpCreateParams);

    auto instancePtr = params->host->ProcessWindowCreation(window);
    params->outInstance = std::move(instancePtr);

    return 0; // success
  }

  if (window != nullhandle) {
    auto instance = reinterpret_cast<Win32Window*>(::GetWindowLongPtrW(window, GWLP_USERDATA));

    if (instance != nullptr) {
      // Pass message to window itself
      return instance->ProcessWin32Message(msg, wparam, lparam);
    }
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
