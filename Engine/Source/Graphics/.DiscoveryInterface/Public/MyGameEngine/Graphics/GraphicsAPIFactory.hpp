#pragma once

#include <MyGameEngine/Graphics/GraphicsAPI.hpp>

/**
 * Factory handles creation of graphics API and its discovery
 */
class GraphicsAPIFactory {
public:
  virtual ~GraphicsAPIFactory() = default;

public:
  virtual GraphicsAPIInformation const& GetInformation() = 0;
  virtual std::expected<std::unique_ptr<GraphicsAPI>, String> CreateAPI() = 0;
};
