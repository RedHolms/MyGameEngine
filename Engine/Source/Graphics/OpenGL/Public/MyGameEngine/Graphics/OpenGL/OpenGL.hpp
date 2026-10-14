#pragma once

#if defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__gl_gl_h_) || defined(__X_GL_H)
#error gl.h included before OpenGL.hpp
#endif
#if defined(__gl2_h_)
#error gl2.h included before OpenGL.hpp
#endif
#if defined(__gltypes_h_)
#error gltypes.h included before OpenGL.hpp
#endif
#if defined(__REGAL_H__)
#error Regal.h included before OpenGL.hpp
#endif
#if defined(__glext_h_) || defined(__GLEXT_H_) || defined(__gl_glext_h_)
#error glext.h included before OpenGL.hpp
#endif
#if defined(__gl_ATI_h_)
#error glATI.h included before OpenGL.hpp
#endif

#define __gl_h_
#define __gl2_h_
#define __GL_H__
#define _GL_H
#define __gl_gl_h_
#define __gltypes_h_
#define __REGAL_H__
#define __X_GL_H
#define __glext_h_
#define __GLEXT_H_
#define __gl_glext_h_
#define __gl_ATI_h_

#include <KHR/khrplatform.h>

#define GL_APIENTRY KHRONOS_APIENTRY

// Defines GL types. Might also include <KHR/khrplatform.h> again but doesn't really matter
#include <MyGameEngine/Generated/OpenGL/Types.generated.hpp>

// Both of them define macros like GL_VERSION_1_0 or GL_3DFX_multisample. Such macros are defined for all
// features/extensions known to the engine compile-time.
#include <MyGameEngine/Generated/OpenGL/Features.generated.hpp>
#include <MyGameEngine/Generated/OpenGL/Extensions.generated.hpp>

// Defines enum values for all enabled features/extensions
#include <MyGameEngine/Generated/OpenGL/Enums.generated.hpp>

#define _COMMAND(NAME, RETURN_TYPE, ...) \
  using NAME ## _t = RETURN_TYPE(GL_APIENTRY*)(__VA_ARGS__);
#include <MyGameEngine/Generated/OpenGL/Commands.generated.def>
#undef _COMMAND

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
