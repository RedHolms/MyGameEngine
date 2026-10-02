#pragma once

#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/String.hpp>
#include <expected>
#include <memory>

class WindowHost {
public:
  static std::expected<std::shared_ptr<WindowHost>, String> Create(String const& uniqueName);

public:
  virtual ~WindowHost() = default;

public:
  /**
   * Creates a simple window with given title and size. Given size is the size of CLIENT area of the window (i.e. window
   * will be actually larger because of titlebar and other decorations). Result contains either a @c SharedPtr<> with
   * created window or a string with error information.
   */
  virtual std::expected<std::shared_ptr<Window>, String> CreateWindow(String const& title, uint width, uint height) = 0;
};
