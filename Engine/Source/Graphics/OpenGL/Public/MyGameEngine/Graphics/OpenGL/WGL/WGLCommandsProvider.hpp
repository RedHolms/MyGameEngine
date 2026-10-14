#pragma once

#include <MyGameEngine/Graphics/OpenGL/WGL/WGL.hpp>

struct WGLCommandsProvider {
#define _COMMAND(NAME, RETURN_TYPE, ...) \
  ::NAME ## _t NAME = nullptr;
#include <MyGameEngine/Generated/OpenGL/WGL/Commands.generated.def>
#undef _COMMAND

#define _FEATURE(NAME) \
  bool has_ ## NAME = false;
#include <MyGameEngine/Generated/OpenGL/WGL/Features.generated.def>
#undef _FEATURE

#define _EXTENSION(NAME) \
  bool has_ ## NAME = false;
#include <MyGameEngine/Generated/OpenGL/WGL/Extensions.generated.def>
#undef _EXTENSION
};
