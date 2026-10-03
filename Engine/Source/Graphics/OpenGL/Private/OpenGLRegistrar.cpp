#include <Private/OpenGLFactory.hpp>
#include <MyGameEngine/Graphics/GraphicsAPIsRegistry.hpp>

struct OpenGLRegistrarType {
  OpenGLRegistrarType() {
    auto& registry = GraphicsAPIsRegistry::GetLazyInstance();

    auto openGlFactory = std::make_unique<OpenGLFactory>(registry.CreateID());
    registry.RegisterFactory(std::move(openGlFactory));
  }
};

static OpenGLRegistrarType OpenGLRegistrar {};
