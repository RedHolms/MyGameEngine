#pragma once

#include <Private/String/Encodings/Base.hpp>

struct Utf32BE_EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) noexcept;
  static String Decode(Bytes const& bytes) noexcept;
};

struct Utf32LE_EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) noexcept;
  static String Decode(Bytes const& bytes) noexcept;
};

template <>
struct EncodingFunctions<Encoding::UTF32_BE> : Utf32BE_EncodingFunctions {};

template <>
struct EncodingFunctions<Encoding::UTF32_LE> : Utf32LE_EncodingFunctions {};
