#pragma once

#include "Base.hpp"

template <Endianness TargetEndianness>
struct Utf32EncodingFunctions {
  static Buffer Encode(const char32_t* string, size_t length) noexcept {
    Buffer result = Buffer::Allocate((length + 1) * sizeof(char32_t));

    if constexpr (Endianness::NATIVE == TargetEndianness) {
      Memory::CopyBytes(result.GetBuffer(), string, length * sizeof(char32_t));
    }
    else {
      for (size_t i = 0; i < length; ++i)
        result.WriteUInt32(i * sizeof(char32_t), string[i], TargetEndianness);
    }

    return result;
  }

  static String Decode(Bytes const& bytes) noexcept {
    String result;
    result.Reserve(bytes.GetSize() / sizeof(char32_t));

    auto rawString = static_cast<const char32_t*>(bytes.GetData());

    for (size_t i = 0; rawString[i] && i < bytes.GetSize(); ++i)
      result.Append(ConventEndiannessToNative(rawString[i], TargetEndianness));

    return result;
  }
};

template <>
struct EncodingFunctions<Encoding::UTF32> : Utf32EncodingFunctions<Endianness::NATIVE> {};

template <>
struct EncodingFunctions<Encoding::UTF32_BE> : Utf32EncodingFunctions<Endianness::BIG> {};

template <>
struct EncodingFunctions<Encoding::UTF32_LE> : Utf32EncodingFunctions<Endianness::LITTLE> {};
