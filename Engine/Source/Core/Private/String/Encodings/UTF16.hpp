#pragma once

#include <Private/String/Encodings/Base.hpp>

struct Utf16BE_EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) noexcept;
  static String Decode(Bytes const& bytes) noexcept;
};

struct Utf16LE_EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) noexcept;
  static String Decode(Bytes const& bytes) noexcept;
};

template <>
struct EncodingFunctions<Encoding::UTF16_BE> : Utf16BE_EncodingFunctions {};

template <>
struct EncodingFunctions<Encoding::UTF16_LE> : Utf16LE_EncodingFunctions {};
