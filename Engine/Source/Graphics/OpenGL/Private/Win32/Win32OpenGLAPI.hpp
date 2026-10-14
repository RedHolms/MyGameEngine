#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>

class Win32OpenGLAPI final : public OpenGLAPI {
public:
  Win32OpenGLAPI();
  ~Win32OpenGLAPI() override;

public:
  std::expected<std::shared_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) override;
};
