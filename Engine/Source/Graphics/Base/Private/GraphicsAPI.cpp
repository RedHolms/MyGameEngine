#include <MyGameEngine/Graphics/GraphicsAPI.hpp>

#include <MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp>

void GraphicsAPI::EnumerateAvailableAPIs(std::function<
  void(GraphicsAPIIdentifier id, GraphicsAPIInformation const& info)> const& callback) noexcept
{
  GraphicsAPIsCatalog::EnumerateProviders([&](auto id, auto provider) {
    GraphicsAPIInformation information {
      .displayName = provider->GetDisplayName()
    };

    callback(id, information);
  });
}

std::expected<std::unique_ptr<GraphicsAPI>, String> GraphicsAPI::Create(GraphicsAPIIdentifier id) {
  GraphicsAPIProvider* provider = nullptr;

  GraphicsAPIsCatalog::EnumerateProviders([&](auto enumId, auto enumProvider) {
    if (provider != nullptr)
      return;

    if (enumId == id) {
      provider = enumProvider;
    }
  });

  if (provider == nullptr)
    return std::unexpected { U"No graphics API with given ID found"_s };

  return provider->Create();
}

GraphicsAPI::~GraphicsAPI() = default;
