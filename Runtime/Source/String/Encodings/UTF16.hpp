#pragma once

#include "Base.hpp"

template <Endianness TargetEndianness>
struct Utf16EncodingFunctions {
  static Buffer Encode(const char32_t* string, size_t length) noexcept {
    // assume worst case (all chars are surrogate pairs)
    Buffer result = Buffer::Allocate((length + 1) * 2 * sizeof(char16_t));

    for (size_t i = 0, j = 0; i < length; ++i) {
      auto codepoint = string[i];
      if (codepoint > 0xFFFF) {
        result.WriteUInt16(j, 0xD7C0 + (codepoint >> 10), TargetEndianness);
        j += sizeof(char16_t);
        result.WriteUInt16(j, 0xDC00 + (codepoint & 0x3FF), TargetEndianness);
        j += sizeof(char16_t);
      }
      else {
        result.WriteUInt16(j, static_cast<char16_t>(codepoint), TargetEndianness);
        j += sizeof(char16_t);
      }
    }

    return result;
  }

  static String Decode(Bytes const& bytes) noexcept {
    String result;

    // assume worst case (all chars are not surrogate pairs)
    result.Reserve(bytes.GetSize() / sizeof(char16_t));

    auto rawString = static_cast<const uint16_t*>(bytes.GetData());

    for (size_t i = 0; rawString[i] && i < bytes.GetSize(); ++i) {
      char32_t codepoint = 0;

      auto high = ConventEndiannessToNative(rawString[i], TargetEndianness);
      if ((high >> 10) == 0b110110) {
        // surrogate pair
        high -= 0xD800;
        auto low = ConventEndiannessToNative(rawString[++i], TargetEndianness) - 0xDC00;
        codepoint = (high << 10) + low + 0x10000;
      }
      else {
        codepoint = static_cast<char32_t>(high);
      }

      result.Append(codepoint);
    }

    return result;
  }
};

template <>
struct EncodingFunctions<Encoding::UTF16> : Utf16EncodingFunctions<Endianness::NATIVE> {};

template <>
struct EncodingFunctions<Encoding::UTF16_BE> : Utf16EncodingFunctions<Endianness::BIG> {};

template <>
struct EncodingFunctions<Encoding::UTF16_LE> : Utf16EncodingFunctions<Endianness::LITTLE> {};
