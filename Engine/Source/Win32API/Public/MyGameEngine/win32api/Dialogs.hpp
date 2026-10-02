#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

constexpr UINT MB_OK = 0x00000000;
constexpr UINT MB_OKCANCEL = 0x00000001;
constexpr UINT MB_ABORTRETRYIGNORE = 0x00000002;
constexpr UINT MB_YESNOCANCEL = 0x00000003;
constexpr UINT MB_YESNO = 0x00000004;
constexpr UINT MB_RETRYCANCEL = 0x00000005;
constexpr UINT MB_CANCELTRYCONTINUE = 0x00000006;
constexpr UINT MB_HELP = 0x00004000;

constexpr UINT MB_ICONHAND = 0x00000010;
constexpr UINT MB_ICONQUESTION = 0x00000020;
constexpr UINT MB_ICONEXCLAMATION = 0x00000030;
constexpr UINT MB_ICONASTERISK = 0x00000040;
constexpr UINT MB_USERICON = 0x00000080;
constexpr UINT MB_ICONWARNING = MB_ICONEXCLAMATION;
constexpr UINT MB_ICONERROR = MB_ICONHAND;
constexpr UINT MB_ICONINFORMATION = MB_ICONASTERISK;
constexpr UINT MB_ICONSTOP = MB_ICONHAND;

constexpr UINT MB_DEFBUTTON1 = 0x00000000;
constexpr UINT MB_DEFBUTTON2 = 0x00000100;
constexpr UINT MB_DEFBUTTON3 = 0x00000200;
constexpr UINT MB_DEFBUTTON4 = 0x00000300;

constexpr UINT MB_APPLMODAL = 0x00000000;
constexpr UINT MB_SYSTEMMODAL = 0x00001000;
constexpr UINT MB_TASKMODAL = 0x00002000;

constexpr UINT MB_NOFOCUS = 0x00008000;
constexpr UINT MB_SETFOREGROUND = 0x00010000;
constexpr UINT MB_DEFAULT_DESKTOP_ONLY = 0x00020000;
constexpr UINT MB_TOPMOST = 0x00040000;
constexpr UINT MB_RIGHT = 0x00080000;
constexpr UINT MB_RTLREADING = 0x00100000;
constexpr UINT MB_SERVICE_NOTIFICATION = 0x00200000;

// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messageboxw
int MessageBoxW(HWND parent, const wchar_t* text, const wchar_t* title, UINT type) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
