add_dependency_component(Engine)
add_dependency_component(Graphics/Base)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(MyGameEngine_Graphics_OpenGL STATIC)

add_library(MyGameEngine_Graphics_OpenGL_WholeArchive INTERFACE)
target_link_libraries(MyGameEngine_Graphics_OpenGL_WholeArchive INTERFACE
  $<LINK_LIBRARY:WHOLE_ARCHIVE,MyGameEngine_Graphics_OpenGL>)

add_library(MyGameEngine::Graphics::OpenGL ALIAS MyGameEngine_Graphics_OpenGL_WholeArchive)

target_sources(MyGameEngine_Graphics_OpenGL PRIVATE
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

target_sources(MyGameEngine_Graphics_OpenGL PUBLIC
  "Public/KHR/khrplatform.h"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGLCommandsProvider.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/WGL/WGLContext.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGL.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLAPI.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLCommandsProvider.hpp"
  "Public/MyGameEngine/Graphics/OpenGL/OpenGLContext.hpp"
)

target_include_directories(MyGameEngine_Graphics_OpenGL
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Public"
)

target_compile_features(MyGameEngine_Graphics_OpenGL PUBLIC cxx_std_26)

target_link_libraries(MyGameEngine_Graphics_OpenGL PUBLIC
  MyGameEngine
  MyGameEngine::Graphics::Base
)

set(GENERATED_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Public/MyGameEngine/Generated")
_mge_touch_generated_directory("${GENERATED_ROOT}")

set(GENERATED_OPENGL_HEADERS
  "${GENERATED_ROOT}/OpenGL/Commands.generated.def"
  "${GENERATED_ROOT}/OpenGL/Enums.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/Extensions.generated.def"
  "${GENERATED_ROOT}/OpenGL/Extensions.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/Features.generated.def"
  "${GENERATED_ROOT}/OpenGL/Features.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/Types.generated.hpp"
)

set(GENERATED_WGL_HEADERS
  "${GENERATED_ROOT}/OpenGL/WGL/Commands.generated.def"
  "${GENERATED_ROOT}/OpenGL/WGL/Enums.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/WGL/Extensions.generated.def"
  "${GENERATED_ROOT}/OpenGL/WGL/Extensions.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/WGL/Features.generated.def"
  "${GENERATED_ROOT}/OpenGL/WGL/Features.generated.hpp"
  "${GENERATED_ROOT}/OpenGL/WGL/Types.generated.hpp"
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
    "${GENERATED_ROOT}/.gitignore"
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
    "${GENERATED_ROOT}/.gitignore"
    "${CMAKE_CURRENT_SOURCE_DIR}/Build/generate-opengl-headers.py"
  COMMENT
    "Generating WGL headers..."
)

add_custom_target(MyGameEngine_GenerateOpenGLHeaders DEPENDS ${GENERATED_OPENGL_HEADERS})
add_custom_target(MyGameEngine_GenerateWGLHeaders DEPENDS ${GENERATED_WGL_HEADERS})

add_dependencies(MyGameEngine_Graphics_OpenGL
  MyGameEngine_GenerateOpenGLHeaders
  MyGameEngine_GenerateWGLHeaders
)
