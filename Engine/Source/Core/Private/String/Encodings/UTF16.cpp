#include <Private/String/Encodings/UTF16.hpp>

template <Endianness TargetEndianness, Encoding Encoding>
struct __Utf16Impl {
  static inline EncodedString Encode(const char32_t* string, size_t length) noexcept {
    // assume worst case (all chars are surrogate pairs)
    Buffer result = Buffer::AllocateGarbaged((length + 1) * 2 * sizeof(char16_t));

    size_t j = 0;
    for (size_t i = 0; i < length; ++i) {
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

    result.WriteUInt16(j, 0, TargetEndianness);

    return EncodedString { Encoding, std::move(result), j };
  }

  static inline String Decode(Bytes const& bytes) noexcept {
    String result;

    auto const count = bytes.GetSize() / sizeof(char16_t);

    // assume worst case (all chars are not surrogate pairs)
    result.Reserve(count);

    auto rawString = static_cast<const char16_t*>(bytes.GetData());

    for (size_t i = 0; i < count && rawString[i]; ++i) {
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

EncodedString Utf16BE_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  return __Utf16Impl<Endianness::BIG, Encoding::UTF16_BE>::Encode(string, length);
}

String Utf16BE_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  return __Utf16Impl<Endianness::BIG, Encoding::UTF16_BE>::Decode(bytes);
}

EncodedString Utf16LE_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  return __Utf16Impl<Endianness::LITTLE, Encoding::UTF16_LE>::Encode(string, length);
}

String Utf16LE_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  return __Utf16Impl<Endianness::LITTLE, Encoding::UTF16_LE>::Decode(bytes);
}
