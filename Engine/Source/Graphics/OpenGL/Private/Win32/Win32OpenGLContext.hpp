#pragma once

#include <MyGameEngine/Graphics/OpenGL/WGL/WGLCommandsProvider.hpp>
#include <MyGameEngine/Graphics/OpenGL/WGL/WGLContext.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLCommandsProvider.hpp>
#include <string_view>
#include <vector>

class Win32OpenGLContext final : public WGLContext, public OpenGLCommandsProvider, public WGLCommandsProvider {
public:
  Win32OpenGLContext(::HGLRC glRc, ::HDC dc);
  ~Win32OpenGLContext() override;

public:
  std::expected<void, String> MakeCurrent() override;
  std::expected<void, String> SwapBuffers() override;

  bool HasExtension(char const* extensionName) override;
  void* GetUnknownCommandProcedure(char const* commandName) override;

  OpenGLCommandsProvider* GetCommandsProvider() override;
  WGLCommandsProvider* GetWGLCommandsProvider() override;

private:
  void LoadCommands();
  void LoadFeatureLevel();
  void LoadExtensionList();
  void LoadWGLSpecificExtensionList();

private:
  ::HGLRC m_glResource;
  ::HDC m_deviceContext;

  std::vector<std::string_view> m_extensionList;
  std::vector<std::string_view> m_wglSpecificExtensionList;
};
