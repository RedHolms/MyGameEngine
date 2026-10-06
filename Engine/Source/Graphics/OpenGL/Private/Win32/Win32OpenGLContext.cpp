#include <Private/Win32/Win32OpenGLContext.hpp>


Win32OpenGLContext::Win32OpenGLContext(HGLRC glRc, HDC dc)
  : m_glResource(glRc),
    m_drawContext(dc)
{}

Win32OpenGLContext::~Win32OpenGLContext() {
  // TODO
}

OpenGLContextTarget Win32OpenGLContext::GetTarget() {
  // TODO add more?
  return OpenGLContextTarget::WINDOW;
}
