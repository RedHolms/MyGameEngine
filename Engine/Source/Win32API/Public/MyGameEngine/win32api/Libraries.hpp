#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

HMODULE GetModuleHandleW(const wchar_t* moduleName) _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace Win32
