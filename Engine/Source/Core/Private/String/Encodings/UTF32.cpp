#include <Private/String/Encodings/UTF32.hpp>

template <Endianness TargetEndianness, Encoding Encoding>
struct __Utf32Impl {
  static inline EncodedString Encode(const char32_t* string, size_t length) noexcept {
    Buffer result = Buffer::Allocate((length + 1) * sizeof(char32_t));

    if constexpr (Endianness::NATIVE == TargetEndianness) {
      MemoryUtils::CopyBytes(result.GetBuffer(), string, length * sizeof(char32_t));
    }
    else {
      for (size_t i = 0; i < length; ++i)
        result.WriteUInt32(i * sizeof(char32_t), string[i], TargetEndianness);
    }

    return EncodedString { Encoding, std::move(result), length };
  }

  static inline String Decode(Bytes const& bytes) noexcept {
    String result;
    auto const count = bytes.GetSize() / sizeof(char32_t);
    result.Reserve(count);

    auto rawString = static_cast<const char32_t*>(bytes.GetData());

    for (size_t i = 0; i < count && rawString[i]; ++i)
      result.Append(ConventEndiannessToNative(rawString[i], TargetEndianness));

    return result;
  }
};

EncodedString Utf32BE_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  return __Utf32Impl<Endianness::BIG, Encoding::UTF16_BE>::Encode(string, length);
}

String Utf32BE_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  return __Utf32Impl<Endianness::BIG, Encoding::UTF16_BE>::Decode(bytes);
}

EncodedString Utf32LE_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  return __Utf32Impl<Endianness::LITTLE, Encoding::UTF16_LE>::Encode(string, length);
}

String Utf32LE_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  return __Utf32Impl<Endianness::LITTLE, Encoding::UTF16_LE>::Decode(bytes);
}
