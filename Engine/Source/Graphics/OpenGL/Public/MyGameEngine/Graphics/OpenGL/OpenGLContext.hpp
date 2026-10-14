#pragma once

#include <MyGameEngine/String.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>
#include <expected>

class OpenGLContext : public OpenGLCommandsProvider {
public:
  virtual ~OpenGLContext() = 0;

public:
  virtual std::expected<void, String> MakeCurrent() = 0;
  virtual std::expected<void, String> SwapBuffers() = 0;

  virtual bool HasExtension(const char* extensionName) = 0;
  virtual void* GetUnknownCommandProcedure(const char* commandName) = 0;
};
