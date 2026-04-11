#pragma once

#include "Core/String.hpp"
#include "Win32Api/Windows.hpp"

/**
 * To keep things away from Microsoft's shit we keep all our windows on a separate thread.
 * This class controls this thread and all our windows.
 */
class WindowsController {
public:
  WindowsController();
  ~WindowsController();

public:
  Win32::WindowHandle CreateWindow(String const& title, uint width, uint height);
};

inline WindowsController* gWindowsController = nullptr;
