#include "Win32Window.hpp"

#include <MyGameEngine/win32api/base.hpp>

Win32Window::Win32Window(Win32WindowHost* parent, ::HWND windowHandle) noexcept
  : m_parent(std::move(parent)),
    m_handle(windowHandle)
{}

Win32Window::~Win32Window() {
  ::DestroyWindow(m_handle);
}

bool Win32Window::IsExternallyDestroyed() {
  return m_handle != nullhandle && ::IsWindow(m_handle);
}

void Win32Window::SetUserCloseRequestCallback(std::function<void()>&& callback) {
  m_userCloseRequestCallback = std::move(callback);
}

void Win32Window::SetExternalDestroyCallback(std::function<void()>&& callback) {
  m_externalDestroyCallback = std::move(callback);
}