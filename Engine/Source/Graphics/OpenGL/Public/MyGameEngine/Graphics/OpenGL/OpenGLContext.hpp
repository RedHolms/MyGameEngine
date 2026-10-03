#pragma once

enum class OpenGLContextTarget {
  // Context is drawing to a window
  WINDOW
};

class OpenGLContext {
public:
  virtual ~OpenGLContext() = default;

public:
  virtual OpenGLContextTarget GetTarget() = 0;
};
