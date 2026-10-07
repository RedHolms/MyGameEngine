#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>

// Defined by 'Engine' component inside <MyGameEngine/GUI/Windows/Window.hpp>
// TODO: Make it something like 'IRenderingTarget' and don't use Window directly
class Window;

/**
 * Provides an interface to OpenGL API. Does not provide an interface to platform-specific commands.
 */
class OpenGLAPI : public GraphicsAPI {
public:
  /**
   * Creates new OpenGL context bound to a window (i.e. context is drawing to a window).
   * TODO: Add ability to specify color format and other options
   */
  virtual std::expected<std::shared_ptr<OpenGLContext>, String> CreateContextForWindow(std::weak_ptr<Window>&& targetWindow) = 0;

  /**
   * Makes specified context the current OpenGL context on the current thread. This function must **ALWAYS** be called
   * before calling any other OpenGL functions. Pass nullptr to unbind any bound context.
   *
   * NOTE: This function will add new reference to the shared pointer, thus context won't be destroyed while it stays
   * bound.
   */
  virtual std::expected<void, String> MakeContextCurrent(std::shared_ptr<OpenGLContext> const& contextSharedPtr) = 0;

  /**
   * Checks if currently bound context has given extension available
   */
  virtual bool HasExtension(const char* extensionId) = 0;

  /**
   * Swaps back and front buffers of the **currently bound context** (i.e. displays drawn frame).
   */
  virtual std::expected<void, String> SwapBuffers() = 0;
};
