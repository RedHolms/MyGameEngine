#pragma once

#ifdef __wglext_h_
#error wglext.h included before WGL.hpp
#endif

#define __wglext_h_

#include <MyGameEngine/Graphics/OpenGL/OpenGL.hpp>
#include <MyGameEngine/Win32Inc.hpp>

#define WGL_APIENTRY KHRONOS_APIENTRY

// Defines WGL types
#include <MyGameEngine/Generated/OpenGL/WGL/Types.generated.hpp>

// Both of them define macros like WGL_VERSION_1_0 or WGL_3DFX_multisample. Such macros are defined for all
// features/extensions known to the engine compile-time.
#include <MyGameEngine/Generated/OpenGL/WGL/Features.generated.hpp>
#include <MyGameEngine/Generated/OpenGL/WGL/Extensions.generated.hpp>

// Defines enum values for all enabled features/extensions
#include <MyGameEngine/Generated/OpenGL/WGL/Enums.generated.hpp>

#define _COMMAND(NAME, RETURN_TYPE, ...) \
  using NAME ## _t = RETURN_TYPE(WGL_APIENTRY*)(__VA_ARGS__);
#include <MyGameEngine/Generated/OpenGL/WGL/Commands.generated.def>
#undef _COMMAND

/**
 * Define types for wingdi.h-provided functions
 */
using ChoosePixelFormat_t = decltype(&::ChoosePixelFormat);
using DescribePixelFormat_t = decltype(&::DescribePixelFormat);
using GetPixelFormat_t = decltype(&::GetPixelFormat);
using SetPixelFormat_t = decltype(&::SetPixelFormat);
using SwapBuffers_t = decltype(&::SwapBuffers);
using wglCopyContext_t = decltype(&::wglCopyContext);
using wglCreateContext_t = decltype(&::wglCreateContext);
using wglCreateLayerContext_t = decltype(&::wglCreateLayerContext);
using wglDeleteContext_t = decltype(&::wglDeleteContext);
using wglDescribeLayerPlane_t = decltype(&::wglDescribeLayerPlane);
using wglGetCurrentContext_t = decltype(&::wglGetCurrentContext);
using wglGetCurrentDC_t = decltype(&::wglGetCurrentDC);
using wglGetLayerPaletteEntries_t = decltype(&::wglGetLayerPaletteEntries);
using wglGetProcAddress_t = decltype(&::wglGetProcAddress);
using wglMakeCurrent_t = decltype(&::wglMakeCurrent);
using wglRealizeLayerPalette_t = decltype(&::wglRealizeLayerPalette);
using wglSetLayerPaletteEntries_t = decltype(&::wglSetLayerPaletteEntries);
using wglShareLists_t = decltype(&::wglShareLists);
using wglSwapLayerBuffers_t = decltype(&::wglSwapLayerBuffers);
using wglUseFontBitmapsA_t = decltype(&::wglUseFontBitmapsA);
using wglUseFontBitmapsW_t = decltype(&::wglUseFontBitmapsW);
using wglUseFontOutlinesA_t = decltype(&::wglUseFontOutlinesA);
using wglUseFontOutlinesW_t = decltype(&::wglUseFontOutlinesW);
