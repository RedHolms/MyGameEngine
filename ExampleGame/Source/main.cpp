#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>
#include <thread>

bool WantToExit = false;

uint GameMain() {
  auto windowHostResult = WindowHost::Create(U"ExampleGameWindowHost"_s);
  if (!windowHostResult.has_value())
    return 1;

  std::shared_ptr<WindowHost> windowHost = std::move(windowHostResult).value();

  auto windowResult = windowHost->CreateWindow(U"ExampleGame Window"_s, 1280, 720);
  if (!windowResult.has_value())
    return 1;

  std::shared_ptr<Window> window = std::move(windowResult).value();

  window->SetExternalDestroyCallback([] {
    WantToExit = true;
  });

  window->SetUserCloseRequestCallback([] {
    WantToExit = true;
  });

  GraphicsAPIIdentifier apiToUse;

  // Keep things stupid simple, use anything we can
  GraphicsAPI::EnumerateAvailableAPIs([&](GraphicsAPIIdentifier id, GraphicsAPIInformation const& info) {
    (void) info;
    apiToUse = id;
  });

  auto graphicsApiResult = GraphicsAPI::Create(apiToUse);
  if (!graphicsApiResult.has_value())
    return 1;

  std::unique_ptr<GraphicsAPI> graphicsApi = std::move(graphicsApiResult).value();

  if (auto openGlApi = dynamic_cast<OpenGLAPI*>(graphicsApi.get())) {
    auto openGlContextResult = openGlApi->CreateContextForWindow(window);
    if (!openGlContextResult.has_value())
      return 1;

    std::shared_ptr<OpenGLContext> openGlContext = std::move(openGlContextResult).value();

    while (!WantToExit) {
      (void) openGlApi->MakeContextCurrent(openGlContext);
      (void) openGlApi->SwapBuffers();
    }

    (void) openGlApi->MakeContextCurrent(nullptr);
  }

  return 0;
}
