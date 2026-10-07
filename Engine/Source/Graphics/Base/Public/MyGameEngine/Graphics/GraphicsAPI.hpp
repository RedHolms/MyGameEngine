#pragma once

#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/String.hpp>
#include <expected>
#include <functional>
#include <memory>

// Unique across all graphics APIs
using GraphicsAPIIdentifier = umword_t;

struct GraphicsAPIInformation {
  String displayName;
  // TODO: More information
};

/**
 * This class represents an interface to graphics API (DirectX, Vulkan, OpenGL, etc.). You can't do really much with
 * this abstract interface. You should dynamic_cast this class to implementation interface (e.g. OpenGLAPI) and use
 * them.
 */
class GraphicsAPI {
public:
  /**
   * Calls @c callback for each available graphics API. By default no graphics API is available. You can add them by
   * enabling engine components inside "Graphics/" directory.
   *
   * TODO: In future we might want to add "dynamic" APIs discovery, i.e. different APIs might be located in DLLs on the
   *  players' PC and we will load them dynamically on the run-time.
   */
  static void EnumerateAvailableAPIs(std::function<
    void(GraphicsAPIIdentifier id, GraphicsAPIInformation const& info)> const& callback) noexcept;

  /**
   * Tries to initialize specified graphics API and create an interface to it. Graphics API ID can be acquired using
   * EnumerateAvailableAPIs() function. Game is responsible for choosing API it wants to use.
   */
  static std::expected<std::unique_ptr<GraphicsAPI>, String> Create(GraphicsAPIIdentifier id);

public:
  virtual ~GraphicsAPI() = 0;
};
