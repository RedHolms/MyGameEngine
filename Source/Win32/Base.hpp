#pragma once

#define WIN32_FUNC __declspec(dllimport)
#define WIN32_API __stdcall

namespace Win32 {

using RAW_HANDLE = void*;
constexpr RAW_HANDLE NULL_HANDLE = nullptr;

} // namespace Win32
