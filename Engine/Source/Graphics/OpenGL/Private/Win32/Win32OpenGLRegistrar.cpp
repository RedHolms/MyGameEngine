#include <Private/Win32/Win32OpenGLFactory.hpp>
#include <MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp>

namespace {

struct Win32OpenGLRegistrarType {
  Win32OpenGLRegistrarType() {
    GraphicsAPIsCatalog::AddProvider(&Win32OpenGLFactory::GetLazyInstance());
  }
};

[[maybe_unused]] Win32OpenGLRegistrarType Win32OpenGLRegistrar {};

} // namespace <anonymous>
