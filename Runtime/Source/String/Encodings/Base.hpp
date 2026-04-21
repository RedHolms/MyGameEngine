#pragma once

#include "Bytes.hpp"

template <Encoding Target>
struct EncodingFunctions {
  static Buffer Encode(const char32_t* string, size_t length) {
    (void)string; (void)length;
    throw RuntimeException(U"Encoding not implemented"s);
  }

  static String Decode(Bytes const& bytes) {
    (void)bytes;
    throw RuntimeException(U"Encoding not implemented"s);
  }
};
