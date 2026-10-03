#include <MyGameEngine/Graphics/GraphicsAPI.hpp>
#include <MyGameEngine/Graphics/GraphicsAPIsRegistry.hpp>

void GraphicsAPI::EnumerateAvailableAPIs(std::function<void(GraphicsAPIInformation const& info)> const& callback) {
  auto& registry = GraphicsAPIsRegistry::GetLazyInstance();

  registry.EnumerateFactories([&](GraphicsAPIFactory* factoryPtr) {
    callback(factoryPtr->GetInformation());
  });
}

std::expected<std::unique_ptr<GraphicsAPI>, String> GraphicsAPI::Create(GraphicsAPIIdentifier id) {
  auto& registry = GraphicsAPIsRegistry::GetLazyInstance();

  GraphicsAPIFactory* requestedFactoryPtr = nullptr;

  registry.EnumerateFactories([&](GraphicsAPIFactory* factoryPtr) {
    if (requestedFactoryPtr != nullptr)
      return;

    if (factoryPtr->GetInformation().id == id)
      requestedFactoryPtr = factoryPtr;
  });

  if (requestedFactoryPtr == nullptr)
    return std::unexpected { U"GraphicsAPI factory with given ID not found"_s };

  return requestedFactoryPtr->CreateAPI();
}
