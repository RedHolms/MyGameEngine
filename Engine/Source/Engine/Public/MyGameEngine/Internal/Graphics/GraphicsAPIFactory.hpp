#pragma once

#include <MyGameEngine/Graphics/GraphicsAPI.hpp>

/**
 * This class represents an interface to a class that provides functions to create some graphics API. Anyone who wants
 * to add new graphics API to an engine must implement this class for wanted API and then register it inside
 * GraphicsAPIsCatalog singleton. Once added to the catalog, unique ID attached to the factory and it becomes available
 * using GraphicsAPI::EnumerateAvailableAPIs() method.
 *
 * When GraphicsAPI::Create() is called with an ID attached to this factory, engine calls Create() method of this
 * factory.
 */
class GraphicsAPIFactory {
public:
  virtual ~GraphicsAPIFactory() = 0;

public:
  virtual String GetDisplayName() = 0;

  virtual std::expected<std::unique_ptr<GraphicsAPI>, String> Create() = 0;
};
