#pragma once

template <typename T>
constexpr bool IsCharType = IsAnyTypeOf<T, char, wchar_t, char8_t, char16_t, char32_t>;

template <typename T>
concept CharType = IsCharType<T>;

// Numeric type capable of containing any Unicode codepoint
using ucs4_t = uint32_t;
static_assert(sizeof(ucs4_t) == 4);
