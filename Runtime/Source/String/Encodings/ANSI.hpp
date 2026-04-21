#pragma once

#include "Base.hpp"
#include "WCHAR.hpp"
#include "Win32/ANSI.hpp"

#define _MAP_CODEPAGE(CODEPAGE, FN, ...)                                                                               \
  case Win32::CodePage::CODEPAGE:                                                                                      \
    return EncodingFunctions<Encoding::CODEPAGE>::FN(__VA_ARGS__)

template <>
struct EncodingFunctions<Encoding::ANSI> {
  static Buffer Encode(const char32_t* string, size_t length) noexcept {
    auto const activeCodePage = Win32::GetActiveCodePage();

    // Try to use our conversion function
    switch (activeCodePage) {
#define _MAP_ENCODE(CODEPAGE) _MAP_CODEPAGE(CODEPAGE, Encode, string, length)
      //TODO...
      //_MAP_ENCODE(CP1251);
#undef _MAP_ENCODE
      default:
        break;
    }

    // fallback to win32 function

    Buffer const wide = EncodingFunctions<Encoding::WCHAR>::Encode(string, length);
    auto const wideBuf = static_cast<const wchar_t*>(wide.GetBuffer());
    auto const wideLen = (wide.GetSize() / sizeof(wchar_t)) - 1;

    size_t requiredSize = Win32::ConvertWideToAnsi(
      activeCodePage,
      Win32::ConvertWideToAnsiFlags::NONE,
      wideBuf, wideLen,
      nullptr, 0,
      nullptr, nullptr
    );

    Buffer result = Buffer::AllocateGarbaged(requiredSize);
    Win32::ConvertWideToAnsi(
      activeCodePage,
      Win32::ConvertWideToAnsiFlags::NONE,
      wideBuf, wideLen,
      static_cast<char*>(result.GetBuffer()), requiredSize,
      nullptr, nullptr
    );

    return result;
  }

  static String Decode(Bytes const& bytes) noexcept {
    auto const activeCodePage = Win32::GetActiveCodePage();

    // Try to use our conversion function
    switch (activeCodePage) {
#define _MAP_DECODE(CODEPAGE) _MAP_CODEPAGE(CODEPAGE, Decode, bytes)
      //TODO...
      //_MAP_DECODE(CP1251);
#undef _MAP_DECODE
      default:
        break;
    }

    // fallback to win32 function

    auto const ansiBuf = static_cast<const char*>(bytes.GetData());
    size_t ansiLen = 0;
    while (ansiLen < bytes.GetSize() && ansiBuf[ansiLen])
      ++ansiLen;

    size_t requiredSize = Win32::ConvertAnsiToWide(
      activeCodePage,
      Win32::ConvertAnsiToWideFlags::NONE,
      ansiBuf, ansiLen,
      nullptr, 0
    );

    Buffer tempWide = Buffer::AllocateGarbaged(requiredSize * sizeof(wchar_t));
    Win32::ConvertAnsiToWide(
      activeCodePage,
      Win32::ConvertAnsiToWideFlags::NONE,
      ansiBuf, ansiLen,
      static_cast<wchar_t*>(tempWide.GetBuffer()), requiredSize
    );

    return EncodingFunctions<Encoding::WCHAR>::Decode(tempWide.GetView());
  }
};

#undef _MAP_CODEPAGE
