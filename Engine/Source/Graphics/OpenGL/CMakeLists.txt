MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  REQUIRE_FULL_LINKAGE
    TRUE
  DEPENDS_ON
    Engine
    Graphics/Base
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

target_sources(${TARGET} PRIVATE
  "Private/Win32/WGLBootstrapper.cpp"
  "Private/Win32/WGLBootstrapper.hpp"
  "Private/Win32/Win32OpenGLFactory.cpp"
  "Private/Win32/Win32OpenGLFactory.hpp"
  "Private/Win32/Win32OpenGLAPI.cpp"
  "Private/Win32/Win32OpenGLAPI.hpp"
  "Private/Win32/Win32OpenGLContext.cpp"
  "Private/Win32/Win32OpenGLContext.hpp"
  "Private/Win32/Win32OpenGLFactory.cpp"
  "Private/Win32/Win32OpenGLFactory.hpp"
  "Private/Win32/Win32OpenGLRegistrar.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/KHR/khrplatform.h"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGLCommandsProvider.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGLContext.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLCommandsProvider.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp"
)

MyGameEngine_MakeGeneratedDirectory("${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated")

set(GENERATED_OPENGL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Commands.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Enums.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Extensions.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Extensions.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Features.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Features.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/Types.generated.hpp"
)

set(GENERATED_WGL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Commands.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Enums.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Extensions.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Extensions.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Features.generated.def"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Features.generated.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL/Types.generated.hpp"
)

add_custom_command(
  OUTPUT
    ${GENERATED_OPENGL_HEADERS}
  COMMAND
    ${Python3_EXECUTABLE}
      "${CMAKE_CURRENT_SOURCE_DIR}/Build/generate-opengl-headers.py"
      "${CMAKE_CURRENT_BINARY_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL"
      "https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/xml/gl.xml"
      "gl"
      "core"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/.gitignore"
    "${CMAKE_CURRENT_SOURCE_DIR}/Build/generate-opengl-headers.py"
  COMMENT
    "Generating OpenGL headers..."
)

add_custom_command(
  OUTPUT
    ${GENERATED_WGL_HEADERS}
  COMMAND
    ${Python3_EXECUTABLE}
      "${CMAKE_CURRENT_SOURCE_DIR}/Build/generate-opengl-headers.py"
      "${CMAKE_CURRENT_BINARY_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/OpenGL/WGL"
      "https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/xml/wgl.xml"
      "wgl"
      "core" # WGL doesn't have profiles but let's specify "core" just in case
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated/.gitignore"
    "${CMAKE_CURRENT_SOURCE_DIR}/Build/generate-opengl-headers.py"
  COMMENT
    "Generating WGL headers..."
)

add_custom_target(MyGameEngine_GenerateOpenGlHeaders DEPENDS ${GENERATED_OPENGL_HEADERS} ${GENERATED_WGL_HEADERS})
add_dependencies(${TARGET} MyGameEngine_GenerateOpenGlHeaders)
