#pragma once

#include <MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp>

class Win32OpenGLFactory final : public GraphicsAPIFactory {
public:
  static Win32OpenGLFactory& GetLazyInstance() {
    static Win32OpenGLFactory Instance;
    return Instance;
  }

private:
  Win32OpenGLFactory();
  ~Win32OpenGLFactory() override;

public:
  String GetDisplayName() override;

  std::expected<std::unique_ptr<GraphicsAPI>, String> Create() override;
};
