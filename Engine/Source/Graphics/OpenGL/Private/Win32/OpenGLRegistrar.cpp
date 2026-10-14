#include <Private/Win32/OpenGLFactory.hpp>
#include <MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp>

namespace {

struct OpenGLRegistrarType {
  OpenGLRegistrarType() {
    GraphicsAPIsCatalog::AddProvider(&OpenGLFactory::GetLazyInstance());
  }
};

[[maybe_unused]] OpenGLRegistrarType OpenGLRegistrar {};

} // namespace <anonymous>
