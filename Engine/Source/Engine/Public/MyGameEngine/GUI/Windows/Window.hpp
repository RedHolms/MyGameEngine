#pragma once

#include <functional>

class Window {
public:
  virtual ~Window() = default;

public:
  virtual bool IsExternallyDestroyed() = 0;

  virtual void SetUserCloseRequestCallback(std::function<void()>&& callback) = 0;
  virtual void SetExternalDestroyCallback(std::function<void()>&& callback) = 0;
};
