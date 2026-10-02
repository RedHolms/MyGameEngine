#pragma once

#include <Private/String/Encodings/Base.hpp>

struct ASCII_EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) noexcept;
  static String Decode(Bytes const& bytes) noexcept;
};

template <>
struct EncodingFunctions<Encoding::ASCII> : ASCII_EncodingFunctions {};
