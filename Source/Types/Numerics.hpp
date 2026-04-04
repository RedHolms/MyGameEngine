#pragma once

using uint = unsigned int;

template <size_t VoidPointerSize>
struct __SelectPlatformDefinedTypes;

template <>
struct __SelectPlatformDefinedTypes<4> {
  using _Unsigned = unsigned long;
  using _Signed = long;
};

template <>
struct __SelectPlatformDefinedTypes<8> {
  using _Unsigned = unsigned long long;
  using _Signed = long long;
};

// Platform-defined types
using __PDTypes = __SelectPlatformDefinedTypes<sizeof(void*)>;

using platform_word_t = __PDTypes::_Unsigned;
