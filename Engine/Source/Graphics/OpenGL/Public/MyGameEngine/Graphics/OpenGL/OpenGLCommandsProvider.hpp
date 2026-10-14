#pragma once

#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>

struct OpenGLCommandsProvider {
#define _COMMAND(NAME, RETURN_TYPE, ...) \
  ::NAME ## _t NAME = nullptr;
#include <MyGameEngine/Generated/OpenGL/Commands.generated.def>
#undef _COMMAND

#define _FEATURE(NAME) \
  bool has_ ## NAME = false;
#include <MyGameEngine/Generated/OpenGL/Features.generated.def>
#undef _FEATURE

#define _EXTENSION(NAME) \
  bool has_ ## NAME = false;
#include <MyGameEngine/Generated/OpenGL/Extensions.generated.def>
#undef _EXTENSION
};
