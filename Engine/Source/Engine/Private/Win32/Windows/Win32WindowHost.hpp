#pragma once

#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <MyGameEngine/win32api/Windows.hpp>
#include <deque>
#include <future>
#include <mutex>
#include <thread>

struct Win32WindowCreationRequest {
  String title;
  uint width;
  uint height;
  std::promise<std::expected<std::shared_ptr<Window>, String>> promise;
};

class Win32WindowHost final : public WindowHost {
  friend WindowHost;

public:
  Win32WindowHost(win32api::HINSTANCE classOwnerInstance, String const& className, win32api::ATOM classAtom) noexcept;
  ~Win32WindowHost() noexcept override;

public:
  std::expected<std::shared_ptr<Window>, String> CreateWindow(String const& title, uint width, uint height) noexcept override;

private:
  void WindowsThreadBody();
  void WindowsThread_ProcessWindowCreationRequests();
  void WindowsThread_ProcessWindowCreationRequest(Win32WindowCreationRequest& request);
  void WindowsThread_ProcessWindowMessages();

private:
  static win32api::LRESULT __stdcall StaticWndProc(win32api::HWND, win32api::UINT, win32api::WPARAM, win32api::LPARAM);

private:
  win32api::HINSTANCE m_classOwnerInstance;
  String m_className;
  win32api::ATOM m_classAtom;

  std::thread m_windowsThread;
  std::atomic_flag m_windowsThreadExitFlag;
  win32api::HANDLE m_windowsThreadWakeUpEvent = win32api::nullhandle;

  std::mutex m_windowCreationRequestQueueMutex;
  std::deque<Win32WindowCreationRequest> m_windowCreationRequestQueue;
};
