#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>

// Defined by 'Engine' component inside <MyGameEngine/GUI/Windows/Window.hpp>
// TODO: Make it something like 'IRenderingTarget' and don't use Window directly
class Window;

/**
 * Provides an interface to OpenGL API
 */
class OpenGLAPI : public GraphicsAPI {
public:
  /**
   * Creates new OpenGL context bound to a window (i.e. context is drawing to a window).
   * TODO: Add ability to specify color format and other options
   */
  virtual std::expected<std::shared_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) = 0;
};
