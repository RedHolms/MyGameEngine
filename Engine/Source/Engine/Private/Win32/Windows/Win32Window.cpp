#include "Win32Window.hpp"

Win32Window::Win32Window(Win32WindowHost* parent, win32api::HWND windowHandle) noexcept
  : m_parent(std::move(parent)),
    m_handle(windowHandle)
{}

Win32Window::~Win32Window() {
  win32api::DestroyWindow(m_handle);
}

bool Win32Window::IsExternallyDestroyed() {
  return m_handle != win32api::nullhandle && win32api::IsWindow(m_handle);
}

void Win32Window::SetUserCloseRequestCallback(std::function<void()>&& callback) {
  m_userCloseRequestCallback = std::move(callback);
}

void Win32Window::SetExternalDestroyCallback(std::function<void()>&& callback) {
  m_externalDestroyCallback = std::move(callback);
}