#include <MyGameEngine/Console.hpp>
#include <MyGameEngine/Logging.hpp>
#include <MyGameEngine/Assets/AssetArchive.hpp>
#include <MyGameEngine/GUI/Windows/Window.hpp>
#include <MyGameEngine/GUI/Windows/WindowHost.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp>
#include <MyGameEngine/Graphics/OpenGL/OpenGLCommandsProvider.hpp>
#include <MyGameEngine/Graphics/GraphicsAPI.hpp>
#include <MyGameEngine/Win32Inc.hpp>
#include <cmath>
#include <array>
#include <filesystem>

bool WantToExit = false;

static const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)glsl";

static const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

uniform vec4 uniColor;

void main()
{
    FragColor = uniColor;
}
)glsl";

static float vertices[] = {
   0.5f,  0.5f, 0.0f, // top right
   0.5f, -0.5f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, // bottom left
  -0.5f,  0.5f, 0.0f  // top left
};
static unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};

static GLuint VAO {}, VBO {}, EBO {};
static GLuint shaderProgram {};
static GLint uniColorLocation {};

static void RenderInit(OpenGLCommandsProvider* gl) {
  int success;
  char infoLog[512];
  GLuint vertexShader {}, fragmentShader {};

  gl->glGenVertexArrays(1, &VAO);
  gl->glBindVertexArray(VAO);

  gl->glGenBuffers(1, &VBO);
  gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
  gl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  gl->glGenBuffers(1, &EBO);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  vertexShader = gl->glCreateShader(GL_VERTEX_SHADER);
  gl->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  gl->glCompileShader(vertexShader);
  gl->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    gl->glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    Log::Error("Vertex shader compilation failed: {}", infoLog);
  }

  fragmentShader = gl->glCreateShader(GL_FRAGMENT_SHADER);
  gl->glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  gl->glCompileShader(fragmentShader);
  gl->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    gl->glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    Log::Error("Fragment shader compilation failed: {}", infoLog);
  }

  shaderProgram = gl->glCreateProgram();
  gl->glAttachShader(shaderProgram, vertexShader);
  gl->glAttachShader(shaderProgram, fragmentShader);
  gl->glLinkProgram(shaderProgram);
  gl->glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    gl->glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    Log::Error("Shader program linkage failed: {}", infoLog);
  }

  gl->glDeleteShader(vertexShader);
  gl->glDeleteShader(fragmentShader);

  gl->glUseProgram(shaderProgram);

  gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
  gl->glEnableVertexAttribArray(0);

  uniColorLocation = gl->glGetUniformLocation(shaderProgram, "uniColor");
}

static void Render(OpenGLCommandsProvider* gl) {
  gl->glClearColor(0.44f, 0.33f, 0.50f, 1.00f);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl->glUseProgram(shaderProgram);
  gl->glBindVertexArray(VAO);

  float timeValue = (std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now())
    .time_since_epoch().count() % 1000000) / 1000.0f;

  float redValue = (sin(timeValue) / 2.0f) + 0.5f;
  gl->glUniform4f(uniColorLocation, redValue, 0.0f, 0.0f, 1.0f);

  gl->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
}

uint GameMain() {
  Console::MakeSureConsolePresent();

  Log::Info("Starting up...");

  std::array<wchar_t, 32768> executablePathBuffer {};
  DWORD const executablePathLength = ::GetModuleFileNameW(nullptr, executablePathBuffer.data(),
    static_cast<DWORD>(executablePathBuffer.size()));
  if (executablePathLength == 0 || executablePathLength >= executablePathBuffer.size()) {
    Log::Error("Unable to locate the executable directory for game assets");
    return 1;
  }

  AssetManager assets;
  auto mountResult = assets.MountArchive(
    std::filesystem::path(executablePathBuffer.data()).parent_path() / "game.mga");
  if (!mountResult) {
    Log::Error("Unable to load game asset archive: {}", mountResult.error());
    return 1;
  }
  if (auto welcome = assets.Find("docs/welcome.txt"))
    Log::Info("Loaded packaged welcome asset ({} bytes)", welcome->GetSize());

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
    auto gl = openGlContext->GetCommandsProvider();

    RenderInit(gl);

    uint framesDrawn = 0;
    auto nextFpsCountResetTimepoint = std::chrono::steady_clock::now() + std::chrono::seconds(1);

    while (!WantToExit) {
      (void) openGlContext->MakeCurrent();
      Render(gl);

      (void) openGlContext->SwapBuffers();
      ++framesDrawn;

      if (std::chrono::steady_clock::now() >= nextFpsCountResetTimepoint) {
        Log::Info("FPS: {}", framesDrawn);
        framesDrawn = 0;
        nextFpsCountResetTimepoint = std::chrono::steady_clock::now() + std::chrono::seconds(1);
      }
    }
  }

  return 0;
}
