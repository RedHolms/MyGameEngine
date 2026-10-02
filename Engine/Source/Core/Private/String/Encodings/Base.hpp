#pragma once

#include <MyGameEngine/EncodedString.hpp>
#include <MyGameEngine/String.hpp>

template <Encoding Target>
struct EncodingFunctions {
  static EncodedString Encode(const char32_t* string, size_t length) {
    (void)string; (void)length;
    static_assert(false, "unimplemented encoding");
  }

  static String Decode(Bytes const& bytes) {
    (void)bytes;
    static_assert(false, "unimplemented encoding");
  }
};
