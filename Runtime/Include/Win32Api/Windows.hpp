#pragma once

#include "Core/String.hpp"
#include "Base.hpp"

namespace Win32 {

using WindowHandle = RAW_HANDLE;

using WindowClassProcedure = intptr_t(__stdcall*)(WindowHandle, uint, uintptr_t, intptr_t);

// Win32 name: RegisterClassExW
uint16_t RegisterWindowClass(
  uint32_t style,
  WindowClassProcedure procedure,
  int classExtra,
  int windowExtra,
  RAW_HANDLE instance,
  RAW_HANDLE icon,
  RAW_HANDLE cursor,
  RAW_HANDLE backgroundBrush,
  String const& menuName,
  String const& className,
  RAW_HANDLE iconSmall
);

// Win32 name: CreateWindowExW
WindowHandle CreateWindow(
  uint32_t exStyle,
  String const& className,
  String const& windowName,
  uint32_t style,
  int x, int y,
  int width, int height,
  WindowHandle parent,
  RAW_HANDLE menu,
  RAW_HANDLE instance,
  void* userdata
);

} // namespace Win32
