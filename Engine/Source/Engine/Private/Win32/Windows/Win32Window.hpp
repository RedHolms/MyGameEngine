#pragma once

#include <Private/Win32/Windows/Win32WindowHost.hpp>
#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/win32api/Windows.hpp>
#include <memory>

class Win32Window final : public Window {
public:
  Win32Window(Win32WindowHost* parent, win32api::HWND windowHandle) noexcept;
  ~Win32Window() override;

public:
  bool IsExternallyDestroyed() override;

  void SetUserCloseRequestCallback(std::function<void()>&& callback) override;
  void SetExternalDestroyCallback(std::function<void()>&& callback) override;

private:
  Win32WindowHost* m_parent;
  win32api::HWND m_handle;
  std::function<void()> m_userCloseRequestCallback;
  std::function<void()> m_externalDestroyCallback;
};
