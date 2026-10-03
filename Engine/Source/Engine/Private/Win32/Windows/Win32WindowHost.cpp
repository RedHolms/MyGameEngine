#include "Win32WindowHost.hpp"

#include <Private/Win32/Windows/Win32Window.hpp>
#include <MyGameEngine/win32api/Libraries.hpp>
#include <MyGameEngine/win32api/Sync.hpp>
#include <MyGameEngine/EncodedString.hpp>

namespace {

struct Win32WindowCreationParameters {
  Win32WindowHost* host;
};

} // namespace <anonymous>

Win32WindowHost::Win32WindowHost(win32api::HINSTANCE classOwnerInstance, String const& className, win32api::ATOM classAtom)
  : m_classOwnerInstance(classOwnerInstance),
    m_className(className),
    m_classAtom(classAtom)
{
  m_windowsThreadWakeUpEvent = win32api::CreateEventW(nullptr, false, false, nullptr);
  m_windowsThread = std::thread { &Win32WindowHost::WindowsThreadBody, this };
}

Win32WindowHost::~Win32WindowHost() noexcept {
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
  win32api::SetEvent(m_windowsThreadWakeUpEvent);

  // Wait for the request to complete
  return future.get();
}

void Win32WindowHost::WindowsThreadBody() {
  win32api::HANDLE handlesToWait[] = { m_windowsThreadWakeUpEvent };

  while (!m_windowsThreadExitFlag.test()) {
    WindowsThread_ProcessWindowCreationRequests();
    WindowsThread_ProcessWindowMessages();

    // Sleep until someone notifies us or new message arrives
    (void) win32api::MsgWaitForMultipleObjectsEx(
      std::size(handlesToWait), handlesToWait,
      win32api::INFINITE,
      win32api::QS_ALLINPUT,
      win32api::MWMO_INPUTAVAILABLE);
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

  auto windowHandle = win32api::CreateWindowExW(
    0,
    reinterpret_cast<const wchar_t*>(m_classAtom),
    titleWChar.GetCString<wchar_t>(),
    win32api::WS_OVERLAPPEDWINDOW,
    win32api::CW_USEDEFAULT,
    win32api::CW_USEDEFAULT,
    request.width,
    request.height,
    win32api::nullhandle,
    win32api::nullhandle,
    m_classOwnerInstance,
    &parameters);

  if (windowHandle == win32api::nullhandle) {
    // TODO: Format win32 error message
    request.promise.set_value(std::unexpected { U"Failed to create a window"_s });
    return;
  }

  win32api::ShowWindow(windowHandle, win32api::SW_SHOW);
  win32api::UpdateWindow(windowHandle);

  auto windowInstance = std::make_shared<Win32Window>(this, windowHandle);
  request.promise.set_value(std::move(windowInstance));
}

void Win32WindowHost::WindowsThread_ProcessWindowMessages() {
  win32api::MSG msg {};
  while (win32api::PeekMessageW(&msg, win32api::nullhandle, 0, 0, win32api::PM_REMOVE)) {
    win32api::TranslateMessage(&msg);
    win32api::DispatchMessageW(&msg);
  }
}

win32api::LRESULT Win32WindowHost::StaticWndProc(win32api::HWND window, win32api::UINT msg, win32api::WPARAM wparam, win32api::LPARAM lparam) {
  switch (msg) {

  }


  // Pass message to Win32
  return win32api::DefWindowProcW(window, msg, wparam, lparam);
}

std::expected<std::shared_ptr<WindowHost>, String> WindowHost::Create(String const& uniqueName) {
  auto classOwnerInstance = win32api::GetModuleHandleW(nullptr);

  // Make a name for the class
  auto className = U"MyGameEngineWindowClass/"_s + uniqueName;
  auto classNameWChar = className.Encode(Encoding::WCHAR);

  win32api::WNDCLASSEXW wndclassexw {
    .cbSize = sizeof(win32api::WNDCLASSEXW),
    .style = 0,
    .lpfnWndProc = &Win32WindowHost::StaticWndProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = classOwnerInstance,
    .hIcon = win32api::nullhandle,
    .hCursor = win32api::nullhandle,
    .hbrBackground = win32api::nullhandle,
    .lpszMenuName = nullptr,
    .lpszClassName = classNameWChar.GetCString<wchar_t>(),
    .hIconSm = win32api::nullhandle
  };

  // Try to register window class
  auto atom = win32api::RegisterClassExW(&wndclassexw);
  if (atom == 0) {
    // Registration failed. TODO: Return formatted win32 error info
    return std::unexpected {U"Failed to create window class"_s};
  }

  // Create an instance of window host implementation
  auto hostInstance = std::make_shared<Win32WindowHost>(classOwnerInstance, className, atom);
  return hostInstance;
}
