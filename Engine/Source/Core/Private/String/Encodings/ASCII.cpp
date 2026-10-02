#include <Private/String/Encodings/ASCII.hpp>

EncodedString ASCII_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  Buffer result = Buffer::Allocate(length + 1);

  for (size_t i = 0; i < length; ++i) {
    auto codepoint = string[i];
    if (codepoint > 0x7F)
      codepoint = '?';
    result.WriteUInt8(i, static_cast<uint8>(codepoint));
  }

  return { Encoding::ASCII, std::move(result), length };
}

String ASCII_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  String result;
  result.Reserve(bytes.GetSize());

  auto rawString = static_cast<const char*>(bytes.GetData());

  for (size_t i = 0; i < bytes.GetSize() && rawString[i]; ++i) {
    if (rawString[i] > 0x7F)
      result.Append(0xFFFD);
    else
      result.Append(rawString[i]);
  }

  return result;
}
