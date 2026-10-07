#include <Private/Win32/Win32OpenGLProvider.hpp>
#include <MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp>

namespace {

struct OpenGLRegistrarType {
  OpenGLRegistrarType() : Provider(std::make_unique<Win32OpenGLProvider>()) {
    GraphicsAPIsCatalog::AddProvider(Provider.get());
  }

  std::unique_ptr<Win32OpenGLProvider> Provider;
};

[[maybe_unused]] OpenGLRegistrarType OpenGLRegistrar {};

} // namespace <anonymous>
