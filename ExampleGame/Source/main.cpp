#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <thread>

uint GameMain() {
  auto windowHostResult = WindowHost::Create(U"ExampleGameWindowHost"_s);
  if (!windowHostResult.has_value())
    return 1;

  std::shared_ptr<WindowHost> windowHost = std::move(windowHostResult).value();

  auto windowResult = windowHost->CreateWindow(U"ExampleGame Window"_s, 1280, 720);
  if (!windowResult.has_value())
    return 1;

  std::shared_ptr<Window> window = std::move(windowResult).value();

  bool wantToExit = false;

  window->SetUserCloseRequestCallback([&wantToExit] {
    wantToExit = true;
  });

  while (!wantToExit)
    std::this_thread::yield();

  return 0;
}
