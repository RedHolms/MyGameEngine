#pragma once

#include <MyGameEngine/String.hpp>
#include <expected>

class OpenGLCommandsProvider;

class OpenGLContext {
public:
  virtual ~OpenGLContext() = 0;

public:
  virtual std::expected<void, String> MakeCurrent() = 0;
  virtual std::expected<void, String> SwapBuffers() = 0;

  virtual bool HasExtension(const char* extensionName) = 0;

  /**
   * NOTE: On Windows, behaviour is undefined if this context is not current
   */
  virtual void* GetUnknownCommandProcedure(const char* commandName) = 0;

  virtual OpenGLCommandsProvider* GetCommandsProvider() = 0;
};
