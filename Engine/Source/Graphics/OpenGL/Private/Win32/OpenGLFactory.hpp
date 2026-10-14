#pragma once

#include <MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp>

class OpenGLFactory final : public GraphicsAPIFactory {
public:
  static OpenGLFactory& GetLazyInstance() {
    static OpenGLFactory Instance;
    return Instance;
  }

private:
  OpenGLFactory();
  ~OpenGLFactory() override;

public:
  String GetDisplayName() override;

  std::expected<std::unique_ptr<GraphicsAPI>, String> Create() override;
};
