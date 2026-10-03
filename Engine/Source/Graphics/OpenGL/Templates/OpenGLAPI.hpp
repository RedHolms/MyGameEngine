#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>
#include <MyGameEngine/GUI/Windows/Window.hpp>

class OpenGLAPI : public GraphicsAPI {
public:
  virtual std::expected<std::unique_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) = 0;
  virtual void MakeContextCurrent(OpenGLContext* context) = 0;

  virtual bool HasExtension(const char* extensionId) = 0;

public: // AUTO-GENERATED COMMANDS
  <<$GLMETHODS$>>
};
