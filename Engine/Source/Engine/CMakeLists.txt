MyGameEngine_ComponentSources(
  PRIVATE
    "Private/Win32/Windows/Win32Window.cpp"
    "Private/Win32/Windows/Win32Window.hpp"
    "Private/Win32/Windows/Win32WindowHost.cpp"
    "Private/Win32/Windows/Win32WindowHost.hpp"
  PUBLIC
    "Public/MyGameEngine/GUI/Windows/Window.hpp"
    "Public/MyGameEngine/GUI/Windows/WindowHost.hpp"
)

MyGameEngine_ComponentDependencies(
  PRIVATE
    Win32API
)
