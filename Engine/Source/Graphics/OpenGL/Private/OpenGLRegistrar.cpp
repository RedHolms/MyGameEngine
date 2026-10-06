#include <Private/OpenGLFactory.hpp>
#include <MyGameEngine/Graphics/GraphicsAPIsRegistry.hpp>

namespace {

struct OpenGLRegistrarType {
  OpenGLRegistrarType() {
    auto& registry = GraphicsAPIsRegistry::GetLazyInstance();

    auto openGlFactory = std::make_unique<OpenGLFactory>(registry.CreateID());
    registry.RegisterFactory(std::move(openGlFactory));
  }
};

[[maybe_unused]] OpenGLRegistrarType OpenGLRegistrar {};

} // namespace <anonymous>
