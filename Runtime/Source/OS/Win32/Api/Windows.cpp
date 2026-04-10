#include "Win32Api/Windows.hpp"

#include "DeclSpecs.hpp"

extern "C" {

struct WNDCLASSEXW {
  uint32_t cbSize;
  uint32_t style;
  Win32::WindowClassProcedure lpfnWndProc;
  int cbClsExtra;
  int cbWndExtra;
  void* hInstance;
  void* hIcon;
  void* hCursor;
  void* hbrBackground;
  const wchar_t* lpszMenuName;
  const wchar_t* lpszClassName;
  void* hIconSm;
};

WIN32_FUNC uint16_t WIN32_API RegisterClassExW(WNDCLASSEXW const*);
WIN32_FUNC Win32::WindowHandle WIN32_API CreateWindowExW(
  uint32_t,
  const wchar_t*,
  const wchar_t*,
  uint32_t,
  int,
  int,
  int,
  int,
  Win32::RAW_HANDLE,
  Win32::RAW_HANDLE,
  Win32::RAW_HANDLE,
  void*
);

} // extern "C"

uint16_t Win32::RegisterWindowClass(
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
) {
  auto menuNameWChar = menuName.Encode(Encoding::WCHAR), classNameWChar = className.Encode(Encoding::WCHAR);
  WNDCLASSEXW wndClassExW {};
  wndClassExW.cbSize = sizeof(wndClassExW);
  wndClassExW.style = style;
  wndClassExW.lpfnWndProc = procedure;
  wndClassExW.cbClsExtra = classExtra;
  wndClassExW.cbWndExtra = windowExtra;
  wndClassExW.hInstance = instance;
  wndClassExW.hIcon = icon;
  wndClassExW.hCursor = cursor;
  wndClassExW.hbrBackground = backgroundBrush;
  wndClassExW.lpszMenuName = static_cast<wchar_t*>(menuNameWChar.GetBuffer());
  wndClassExW.lpszClassName = static_cast<wchar_t*>(classNameWChar.GetBuffer());
  wndClassExW.hIconSm = iconSmall;
  return ::RegisterClassExW(&wndClassExW);
}

Win32::WindowHandle Win32::CreateWindow(
  uint32_t exStyle,
  String const& className,
  String const& windowName,
  uint32_t style,
  int x,
  int y,
  int width,
  int height,
  WindowHandle parent,
  RAW_HANDLE menu,
  RAW_HANDLE instance,
  void* userdata
) {
  auto classNameWChar = className.Encode(Encoding::WCHAR), windowNameWChar = windowName.Encode(Encoding::WCHAR);
  return ::CreateWindowExW(
    exStyle,
    static_cast<wchar_t*>(classNameWChar.GetBuffer()),
    static_cast<wchar_t*>(windowNameWChar.GetBuffer()),
    style,
    x,
    y,
    width,
    height,
    parent,
    menu,
    instance,
    userdata
  );
}
