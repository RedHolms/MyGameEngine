#pragma once

/**
 * This class represents an interface to graphics API context (e.g. ID3D12Device, OpenGL context, etc.). You can't do
 * really much with this abstract interface. You should dynamic_cast this class to implementation interface (e.g.
 * OpenGLContext) and use them.
 */
class GraphicsContext {
public:
  virtual ~GraphicsContext() = 0;
};
