#include <Private/String/Encodings/UTF8.hpp>

static constexpr uint ACCEPT = 0;
static constexpr uint REJECT = 1;

static constexpr uint8 utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

static inline uint32 _utf8_decode_step(uint32* state, char32_t* codep, uint32 byte) noexcept {
  uint32 type = utf8d[byte];
  uint32 st = *state;

  if (codep != nullptr) {
    *codep = (st != 0) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
  }

  return *state = utf8d[256 + st * 16 + type];
}

EncodedString Utf8_EncodingFunctions::Encode(const char32_t* string, size_t length) noexcept {
  Buffer buffer = Buffer::AllocateGarbaged((length * 4) + 1);

  size_t j = 0;
  for (size_t i = 0; i < length; ++i) {
    auto const codepoint = string[i];
    if (codepoint <= 0x7F) {
      buffer.WriteUInt8(j, static_cast<uint8>(codepoint));
      ++j;
    }
    else if (codepoint <= 0x7FF) {
      buffer.WriteUInt8(j, (codepoint >> 6) | 0xC0);
      ++j;
      buffer.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
      ++j;
    }
    else if (codepoint <= 0xFFFF) {
      buffer.WriteUInt8(j, (codepoint >> 12) | 0xE0);
      ++j;
      buffer.WriteUInt8(j, ((codepoint >> 6) & 0x3F) | 0x80);
      ++j;
      buffer.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
      ++j;
    }
    else if (codepoint <= 0x10FFFF) {
      buffer.WriteUInt8(j, (codepoint >> 18) | 0xF0);
      ++j;
      buffer.WriteUInt8(j, ((codepoint >> 12) & 0x3F) | 0x80);
      ++j;
      buffer.WriteUInt8(j, ((codepoint >> 6) & 0x3F) | 0x80);
      ++j;
      buffer.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
      ++j;
    }
  }

  buffer.WriteUInt8(j, 0);

  return EncodedString { Encoding::UTF8, std::move(buffer), j };
}

String Utf8_EncodingFunctions::Decode(Bytes const& bytes) noexcept {
  String result;
  result.Reserve(bytes.GetSize());

  auto rawString = static_cast<const uint8*>(bytes.GetData());

  uint32 state = 0;
  char32_t codepoint = 0;
  for (size_t i = 0; i < bytes.GetSize() && rawString[i]; ++i) {
    switch (_utf8_decode_step(&state, &codepoint, rawString[i])) {
      case ACCEPT:
        result.Append(codepoint);
        codepoint = state = 0;
        break;
      case REJECT:
        if (!rawString[i])
          break;

        result.Append(0xFFFD);
        codepoint = state = 0;
        break;
    }
  }

  return result;
}
