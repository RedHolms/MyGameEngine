#include <Private/Win32/Win32OpenGLFactory.hpp>

#include <Private/Win32/WGLBootstrapper.hpp>
#include <Private/Win32/Win32OpenGLAPI.hpp>

Win32OpenGLFactory::Win32OpenGLFactory() = default;
Win32OpenGLFactory::~Win32OpenGLFactory() = default;

String Win32OpenGLFactory::GetDisplayName() {
  return U"OpenGL"_s;
}

std::expected<std::unique_ptr<GraphicsAPI>, String> Win32OpenGLFactory::Create() {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();

  if (auto result = bootstrapper.MakeSureFunctionsLoaded(); !result.has_value())
    return std::unexpected { std::move(result).error() };

  return std::make_unique<Win32OpenGLAPI>();
}
