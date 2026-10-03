#pragma once

#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/String.hpp>
#include <expected>
#include <functional>
#include <memory>

using GraphicsAPIIdentifier = umword_t;

struct GraphicsAPIInformation {
  GraphicsAPIIdentifier id;
  String displayName;
};

/**
 * This class represents an interface to graphics API (DirectX, Vulkan, OpenGL, etc.)
 */
class GraphicsAPI {
public:
  static void EnumerateAvailableAPIs(std::function<void(GraphicsAPIInformation const& info)> const& callback);

  static std::expected<std::unique_ptr<GraphicsAPI>, String> Create(GraphicsAPIIdentifier id);

public:
  virtual ~GraphicsAPI() = default;

public:
  virtual GraphicsAPIInformation const& GetInformation() = 0;
};
