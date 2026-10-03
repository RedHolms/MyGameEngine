#pragma once

#include <MyGameEngine/Graphics/GraphicsAPIFactory.hpp>
#include <memory>
#include <vector>

/**
 * Registry for all graphics API factories. Implementations must call
 * `GraphicsAPIsRegistry::GetLazyInstance().RegisterFactory(...);` before main function.
 */
class GraphicsAPIsRegistry final {
public:
  static GraphicsAPIsRegistry& GetLazyInstance() {
    static GraphicsAPIsRegistry _Instance;
    return _Instance;
  }

private:
  GraphicsAPIsRegistry() = default;

public:
  inline GraphicsAPIIdentifier CreateID() noexcept {
    return m_firstFreeId++;
  }

  inline void RegisterFactory(std::unique_ptr<GraphicsAPIFactory>&& factory) {
    m_factories.push_back(std::move(factory));
  }

  template <class CallableT>
  inline void EnumerateFactories(CallableT&& callback) const {
    for (auto const& factoryPtr : m_factories)
      std::forward<CallableT>(callback)(factoryPtr.get());
  }

private:
  GraphicsAPIIdentifier m_firstFreeId = 0;
  std::vector<std::unique_ptr<GraphicsAPIFactory>> m_factories;
};
