#include <MyGameEngine/Internal/Graphics/GraphicsAPIsCatalog.hpp>

#include <mutex>
#include <map>

static constinit std::mutex s_GraphicsAPIsCatalogMutex;
static constinit GraphicsAPIIdentifier s_FirstFreeGraphicsAPIIdentifier = 1;
static constinit std::map<GraphicsAPIIdentifier, GraphicsAPIFactory*>* s_GraphicsAPIsMap = nullptr;

GraphicsAPIIdentifier GraphicsAPIsCatalog::AddProvider(GraphicsAPIFactory* provider) {
  std::lock_guard lockGuard { s_GraphicsAPIsCatalogMutex };

  GraphicsAPIIdentifier id = s_FirstFreeGraphicsAPIIdentifier++;

  if (s_GraphicsAPIsMap == nullptr)
    s_GraphicsAPIsMap = new std::map<GraphicsAPIIdentifier, GraphicsAPIFactory*>();

  s_GraphicsAPIsMap->emplace(id, provider);
  return id;
}

void GraphicsAPIsCatalog::RemoveProvider(GraphicsAPIIdentifier providerId) {
  std::lock_guard lockGuard { s_GraphicsAPIsCatalogMutex };

  if (s_GraphicsAPIsMap == nullptr)
    return;

  s_GraphicsAPIsMap->erase(providerId);
}

void GraphicsAPIsCatalog::EnumerateProviders(std::function<void(GraphicsAPIIdentifier id, GraphicsAPIFactory* provider)> const& callback) {
  std::lock_guard lockGuard { s_GraphicsAPIsCatalogMutex };

  if (s_GraphicsAPIsMap == nullptr)
    return;

  for (auto const& [id, provider] : *s_GraphicsAPIsMap)
    callback(id, provider);
}
