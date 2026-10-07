#pragma once

#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <deque>
#include <future>
#include <mutex>
#include <thread>

// HINSTANCE, HATOM, LRESULT, HWND, UINT, WPARAM, LPARAM, HANDLE
#include <MyGameEngine/win32api/base.hpp>

struct Win32WindowCreationRequest {
  String title;
  uint width;
  uint height;
  std::promise<std::expected<std::shared_ptr<Window>, String>> promise;
};

class Win32Window;

class Win32WindowHost final : public WindowHost {
  friend WindowHost;

public:
  Win32WindowHost(::HINSTANCE classOwnerInstance, String const& className, ::ATOM classAtom);
  ~Win32WindowHost() override;

public:
  std::expected<std::shared_ptr<Window>, String> CreateWindow(String const& title, uint width, uint height) override;

private:
  void WindowsThreadBody();
  void WindowsThread_ProcessWindowCreationRequests();
  void WindowsThread_ProcessWindowCreationRequest(Win32WindowCreationRequest& request);
  void WindowsThread_ProcessWindowMessages();

  std::shared_ptr<Win32Window> ProcessWindowCreation(::HWND handle);

private:
  static ::LRESULT __stdcall StaticWndProc(::HWND, ::UINT, ::WPARAM, ::LPARAM);

private:
  ::HINSTANCE m_classOwnerInstance;
  String m_className;
  ::ATOM m_classAtom;

  std::thread m_windowsThread;
  std::atomic_flag m_windowsThreadExitFlag;
  ::HANDLE m_windowsThreadWakeUpEvent = nullhandle;

  std::mutex m_windowCreationRequestQueueMutex;
  std::deque<Win32WindowCreationRequest> m_windowCreationRequestQueue;
};
