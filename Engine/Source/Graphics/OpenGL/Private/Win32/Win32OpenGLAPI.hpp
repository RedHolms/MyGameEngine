#pragma once

#include <Private/Win32/Win32OpenGLContext.hpp>
#include <Private/Win32/Win32OpenGLProvider.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>

class Win32OpenGLAPI final : public OpenGLAPI {
public:
  explicit Win32OpenGLAPI(Win32OpenGLProvider* provider);
  ~Win32OpenGLAPI() override;

public:
  std::expected<std::shared_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) override;
  std::expected<void, String> MakeContextCurrent(std::shared_ptr<OpenGLContext> const& contextSharedPtr) override;

  bool HasExtension(char const* extensionId) override;

  std::expected<void, String> SwapBuffers() override;

private:
  Win32OpenGLProvider* m_provider;
  std::shared_ptr<Win32OpenGLContext> m_boundContext;
};
