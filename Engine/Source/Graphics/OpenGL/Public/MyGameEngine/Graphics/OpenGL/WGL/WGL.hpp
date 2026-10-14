#pragma once

#ifdef __wglext_h_
#error wglext.h included before WGL.hpp
#endif

#define __wglext_h_

#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>
#include <MyGameEngine/Win32Inc.hpp>

#define GL_APIENTRY KHRONOS_APIENTRY

// Defines WGL types
#include <MyGameEngine/Generated/OpenGL/WGL/Types.generated.hpp>

// Both of them define macros like WGL_VERSION_1_0 or WGL_3DFX_multisample. Such macros are defined for all
// features/extensions known to the engine compile-time.
#include <MyGameEngine/Generated/OpenGL/WGL/Features.generated.hpp>
#include <MyGameEngine/Generated/OpenGL/WGL/Extensions.generated.hpp>

// Defines enum values for all enabled features/extensions
#include <MyGameEngine/Generated/OpenGL/WGL/Enums.generated.hpp>

#define _COMMAND(NAME, RETURN_TYPE, ...) \
  using NAME ## _t = RETURN_TYPE(GL_APIENTRY*)(__VA_ARGS__);
#include <MyGameEngine/Generated/OpenGL/WGL/Commands.generated.def>
#undef _COMMAND

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
