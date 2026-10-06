#pragma once

#include <MyGameEngine/Internal/Win32/GUI/Windows/Win32WindowHost.hpp>
#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <memory>

// HWND
#include <MyGameEngine/win32api/base.hpp>

class Win32Window final : public Window {
public:
  Win32Window(Win32WindowHost* parent, ::HWND windowHandle) noexcept;
  ~Win32Window() override;

public:
  bool IsExternallyDestroyed() override;

  void SetUserCloseRequestCallback(std::function<void()>&& callback) override;
  void SetExternalDestroyCallback(std::function<void()>&& callback) override;

public: // Win32Window-specific methods
  ::HWND GetWin32Handle() const noexcept {
    return m_handle;
  }

private:
  Win32WindowHost* m_parent;
  ::HWND m_handle;
  std::function<void()> m_userCloseRequestCallback;
  std::function<void()> m_externalDestroyCallback;
};
