#include <MyGameEngine/Logging.hpp>
#include <MyGameEngine/Logging/Configuration.hpp>
#include <MyGameEngine/Logging/Appenders/ConsoleAppender.hpp>
#include <MyGameEngine/Logging/Formatters/FunctionalFormatter.hpp>
#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>
#include <thread>

bool WantToExit = false;

uint GameMain() {
  Logging::Configure(
    Logging::ConfigurationBuilder()
      .BuildAppender("stdout", Logging::ConsoleAppenderBuilder().SetFile(Logging::ConsoleOutputFile::Standard))
      .BuildAppender("stderr", Logging::ConsoleAppenderBuilder().SetFile(Logging::ConsoleOutputFile::Error))
      .BuildFormatter("default",
        Logging::FunctionalFormatterBuilder()
          .SetFunction([](LoggingRecord const& record, Logging::FormattingContext const& context) -> std::string {
            using namespace std::chrono;

            auto sendTime = floor<microseconds>(zoned_time(current_zone(), record.timepoint).get_local_time());
            auto prefix = fmt::format("[{:%H:%M:%S}]", sendTime);

            if (context.supportsEscapeSequences) {
              return fmt::format("\x1B[90m{}\x1B[39m {}", prefix, record.textUtf8);
            }
            else {
              return fmt::format("{} {}", prefix, record.textUtf8);
            }
          })
      )
      .BeginGroup()
        .SetCondition([](LoggingRecord const& record) -> bool { return record.verbosityLevel < VerbosityLevel::ERROR; })
        .SetFormatter("default")
        .AppendTo("stdout")
      .EndGroup()
      .BeginGroup()
        .SetCondition([](LoggingRecord const& record) -> bool { return record.verbosityLevel >= VerbosityLevel::ERROR; })
        .SetFormatter("default")
        .AppendTo("stderr")
      .EndGroup()
  );

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
