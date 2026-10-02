#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

typedef unsigned short ATOM;

typedef LONG_PTR LRESULT;
typedef LONG_PTR LPARAM;
typedef UINT_PTR WPARAM;

using WNDPROC = LRESULT(__stdcall*)(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

LRESULT DefWindowProcW(HWND window, UINT message, WPARAM wparam, LPARAM lparam) _WIN32_FUNC_ATTRS;

struct WNDCLASSEXW {
  UINT            cbSize;
  UINT            style;
  WNDPROC         lpfnWndProc;
  int             cbClsExtra;
  int             cbWndExtra;
  HINSTANCE       hInstance;
  HICON           hIcon;
  HCURSOR         hCursor;
  HBRUSH          hbrBackground;
  const wchar_t*  lpszMenuName;
  const wchar_t*  lpszClassName;
  HICON           hIconSm;
};

ATOM RegisterClassExW(WNDCLASSEXW const*) _WIN32_FUNC_ATTRS;

constexpr DWORD WS_OVERLAPPED   = 0x00000000L;
constexpr DWORD WS_POPUP        = 0x80000000L;
constexpr DWORD WS_CHILD        = 0x40000000L;
constexpr DWORD WS_MINIMIZE     = 0x20000000L;
constexpr DWORD WS_VISIBLE      = 0x10000000L;
constexpr DWORD WS_DISABLED     = 0x08000000L;
constexpr DWORD WS_CLIPSIBLINGS = 0x04000000L;
constexpr DWORD WS_CLIPCHILDREN = 0x02000000L;
constexpr DWORD WS_MAXIMIZE     = 0x01000000L;
constexpr DWORD WS_BORDER       = 0x00800000L;
constexpr DWORD WS_DLGFRAME     = 0x00400000L;
constexpr DWORD WS_CAPTION      = WS_BORDER | WS_DLGFRAME;
constexpr DWORD WS_VSCROLL      = 0x00200000L;
constexpr DWORD WS_HSCROLL      = 0x00100000L;
constexpr DWORD WS_SYSMENU      = 0x00080000L;
constexpr DWORD WS_THICKFRAME   = 0x00040000L;
constexpr DWORD WS_GROUP        = 0x00020000L;
constexpr DWORD WS_TABSTOP      = 0x00010000L;
constexpr DWORD WS_MINIMIZEBOX  = 0x00020000L;
constexpr DWORD WS_MAXIMIZEBOX  = 0x00010000L;

constexpr DWORD WS_OVERLAPPEDWINDOW =
  WS_OVERLAPPED
  | WS_CAPTION
  | WS_SYSMENU
  | WS_THICKFRAME
  | WS_MINIMIZEBOX
  | WS_MAXIMIZEBOX;

constexpr DWORD WS_POPUPWINDOW =
  WS_POPUP
  | WS_BORDER
  | WS_SYSMENU;

constexpr DWORD WS_CHILDWINDOW  = WS_CHILD;

constexpr DWORD WS_TILED        = WS_OVERLAPPED;
constexpr DWORD WS_ICONIC       = WS_MINIMIZE;
constexpr DWORD WS_SIZEBOX      = WS_THICKFRAME;
constexpr DWORD WS_TILEDWINDOW  = WS_OVERLAPPEDWINDOW;

constexpr int CW_USEDEFAULT = 0x80000000;

HWND CreateWindowExW(
  DWORD           styleEx,
  const wchar_t*  className,
  const wchar_t*  windowName,
  DWORD           style,
  int             x,
  int             y,
  int             width,
  int             height,
  HWND            parent,
  HMENU           menu,
  HINSTANCE       instance,
  LPVOID          param
) _WIN32_FUNC_ATTRS;

constexpr DWORD SW_HIDE            = 0;
constexpr DWORD SW_SHOWNORMAL      = 1;
constexpr DWORD SW_NORMAL          = 1;
constexpr DWORD SW_SHOWMINIMIZED   = 2;
constexpr DWORD SW_SHOWMAXIMIZED   = 3;
constexpr DWORD SW_MAXIMIZE        = 3;
constexpr DWORD SW_SHOWNOACTIVATE  = 4;
constexpr DWORD SW_SHOW            = 5;
constexpr DWORD SW_MINIMIZE        = 6;
constexpr DWORD SW_SHOWMINNOACTIVE = 7;
constexpr DWORD SW_SHOWNA          = 8;
constexpr DWORD SW_RESTORE         = 9;
constexpr DWORD SW_SHOWDEFAULT     = 10;
constexpr DWORD SW_FORCEMINIMIZE   = 11;
constexpr DWORD SW_MAX             = 11;

BOOL IsWindow(HWND window) _WIN32_FUNC_ATTRS;
BOOL ShowWindow(HWND window, int cmd) _WIN32_FUNC_ATTRS;
BOOL DestroyWindow(HWND window) _WIN32_FUNC_ATTRS;
BOOL UpdateWindow(HWND window) _WIN32_FUNC_ATTRS;

constexpr DWORD QS_KEY = 0x0001;
constexpr DWORD QS_MOUSEMOVE = 0x0002;
constexpr DWORD QS_MOUSEBUTTON = 0x0004;
constexpr DWORD QS_POSTMESSAGE = 0x0008;
constexpr DWORD QS_TIMER = 0x0010;
constexpr DWORD QS_PAINT = 0x0020;
constexpr DWORD QS_SENDMESSAGE = 0x0040;
constexpr DWORD QS_HOTKEY = 0x0080;
constexpr DWORD QS_ALLPOSTMESSAGE = 0x0100;
constexpr DWORD QS_RAWINPUT = 0x0400;
constexpr DWORD QS_TOUCH = 0x0800;
constexpr DWORD QS_POINTER = 0x1000;

constexpr DWORD QS_MOUSE = QS_MOUSEMOVE | QS_MOUSEBUTTON;
constexpr DWORD QS_INPUT = QS_MOUSE | QS_KEY | QS_RAWINPUT | QS_TOUCH | QS_POINTER;
constexpr DWORD QS_ALLEVENTS = QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY;
constexpr DWORD QS_ALLINPUT = QS_INPUT | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_HOTKEY | QS_SENDMESSAGE;

constexpr DWORD MWMO_ALERTABLE = 0x0002;
constexpr DWORD MWMO_INPUTAVAILABLE = 0x0004;
constexpr DWORD MWMO_WAITALL = 0x0001;

DWORD MsgWaitForMultipleObjectsEx(
  DWORD count, HANDLE const* handles,
  DWORD milliseconds, DWORD wakeMask, DWORD flags) _WIN32_FUNC_ATTRS;

struct MSG {
  HWND   hwnd;
  UINT   message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD  time;
  POINT  pt;
  DWORD  lPrivate;
};

constexpr DWORD PM_NOREMOVE = 0x0000;
constexpr DWORD PM_REMOVE = 0x0001;
constexpr DWORD PM_NOYIELD = 0x0002;

constexpr DWORD PM_QS_INPUT = QS_INPUT << 16;
constexpr DWORD PM_QS_POSTMESSAGE = (QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER) << 16;
constexpr DWORD PM_QS_PAINT = QS_PAINT << 16;
constexpr DWORD PM_QS_SENDMESSAGE = QS_SENDMESSAGE << 16;

BOOL PeekMessageW(MSG* msg, HWND window, UINT msgFilterMin, UINT msgFilterMax, UINT removeMsg) _WIN32_FUNC_ATTRS;
BOOL TranslateMessage(MSG const* msg) _WIN32_FUNC_ATTRS;
LRESULT DispatchMessageW(MSG const* msg) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
