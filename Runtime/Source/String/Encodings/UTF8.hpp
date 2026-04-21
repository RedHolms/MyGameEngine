#pragma once

#include "Base.hpp"

template <>
struct EncodingFunctions<Encoding::UTF8> {
  static constexpr uint ACCEPT = 0;
  static constexpr uint REJECT = 1;

  static constexpr uint8_t utf8d[] = {
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

  static forceinline uint32_t _DecodeStep(uint32_t* state, char32_t* codep, uint32_t byte) noexcept {
    uint32_t type = utf8d[byte];
    uint32_t st = *state;

    if (codep != nullptr) {
      *codep = (st != 0) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
    }

    return *state = utf8d[256 + st * 16 + type];
  }

  static Buffer Encode(const char32_t* string, size_t length) noexcept {
    Buffer result = Buffer::Allocate((length * 5) + 1);

    for (size_t i = 0, j = 0; i < length; ++i) {
      auto const codepoint = string[i];
      if (codepoint <= 0x7F) {
        result.WriteUInt8(j, static_cast<uint8_t>(codepoint));
        ++j;
      }
      else if (codepoint <= 0x7FF) {
        result.WriteUInt8(j, (codepoint >> 6) | 0xC0);
        ++j;
        result.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
        ++j;
      }
      else if (codepoint <= 0xFFFF) {
        result.WriteUInt8(j, (codepoint >> 12) | 0xE0);
        ++j;
        result.WriteUInt8(j, ((codepoint >> 6) & 0x3F) | 0x80);
        ++j;
        result.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
        ++j;
      }
      else if (codepoint <= 0x10FFFF) {
        result.WriteUInt8(j, (codepoint >> 18) | 0xF0);
        ++j;
        result.WriteUInt8(j, ((codepoint >> 12) & 0x3F) | 0x80);
        ++j;
        result.WriteUInt8(j, ((codepoint >> 6) & 0x3F) | 0x80);
        ++j;
        result.WriteUInt8(j, (codepoint & 0x3F) | 0x80);
        ++j;
      }
    }

    return result;
  }

  static String Decode(Bytes const& bytes) noexcept {
    String result;
    result.Reserve(bytes.GetSize());

    auto rawString = static_cast<const uint8_t*>(bytes.GetData());

    uint32_t state = 0;
    char32_t codepoint = 0;
    for (size_t i = 0; rawString[i] && i < bytes.GetSize(); ++i) {
      switch (_DecodeStep(&state, &codepoint, rawString[i])) {
        case ACCEPT:
          ++i;
          result.Append(codepoint);
          codepoint = state = 0;
          break;
        case REJECT:
          if (!rawString[i])
            break;

          ++i;
          result.Append(0xFFFD);
          codepoint = state = 0;
          break;
      }
    }

    return result;
  }
};
