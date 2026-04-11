#include "Window.hpp"

#include "Win32Api/Windows.hpp"

struct Window::Data {
  Win32::WindowHandle handle = Win32::NULL_HANDLE;
};

Window::Window(String const& title, uint width, uint height) {}

Window::~Window() {}
