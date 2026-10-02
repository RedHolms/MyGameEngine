#include <Private/String/Encodings/ASCII.hpp>
#include <Private/String/Encodings/UTF8.hpp>
#include <Private/String/Encodings/UTF16.hpp>
#include <Private/String/Encodings/UTF32.hpp>
#include <Private/String/Encodings/WCHAR.hpp>
#include <MyGameEngine/EncodedString.hpp>
#include <MyGameEngine/String.hpp>

EncodedString String::Encode(Encoding encoding) const {
  switch (encoding) {
#define _ENCODING(NAME, _UNIT_SIZE)                                                                                    \
  case Encoding::NAME:                                                                                                 \
    return EncodingFunctions<Encoding::NAME>::Encode(m_buffer, m_length);
#include <MyGameEngine/__string_encodings.def>
#undef _ENCODING
  }

  throw std::runtime_error("Invalid encoding passed to String::Encode()");
}

String String::Decode(Bytes const& bytes, Encoding encoding) {
  switch (encoding) {
#define _ENCODING(NAME, _UNIT_SIZE)                                                                                    \
  case Encoding::NAME:                                                                                                 \
    return EncodingFunctions<Encoding::NAME>::Decode(bytes);
#include <MyGameEngine/__string_encodings.def>
#undef _ENCODING
  }

  throw std::runtime_error("Invalid encoding passed to String::Decode()");
}
