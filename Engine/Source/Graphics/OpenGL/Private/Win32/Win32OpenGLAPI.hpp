#pragma once

#include <MyGameEngine/Graphics/OpenGL/WGLAPI.hpp>

class Win32OpenGLAPI final : public WGLAPI {
public:
  ~Win32OpenGLAPI() override;

public:
  GraphicsAPIInformation const& GetInformation() override;

  std::expected<std::unique_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) override;
  void MakeContextCurrent(OpenGLContext* context) override;

  bool HasExtension(char const* extensionId) override;
};
