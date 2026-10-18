#pragma once

#include <MyGameEngine/Internal/Graphics/GraphicsAPIFactory.hpp>
#include <functional>

/**
 * Provides a way to register new graphics API providers and enumerate them
 */
class GraphicsAPIsCatalog final {
public:
  /**
   * Adds new provider to the catalog. Provider MUST live as long as it persists in the catalog. Returns an ID attached
   * to the provider.
   */
  static GraphicsAPIIdentifier AddProvider(GraphicsAPIFactory* provider);

  /**
   * Removes provider from the catalog by its ID
   */
  static void RemoveProvider(GraphicsAPIIdentifier providerId);

  /**
   * Calls @c callback over all registered providers
   */
  static void EnumerateProviders(std::function<void(GraphicsAPIIdentifier id, GraphicsAPIFactory* provider)> const& callback);
};
