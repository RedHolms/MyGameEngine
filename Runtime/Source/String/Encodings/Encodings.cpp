#include "UTF8.hpp"
#include "UTF16.hpp"
#include "UTF32.hpp"
#include "WCHAR.hpp"

Buffer String::Encode(Encoding encoding) const {
  switch (encoding) {
#define _ENCODING(NAME)                                                                                                \
  case Encoding::NAME:                                                                                                 \
    return EncodingFunctions<Encoding::NAME>::Encode(m_buffer, m_length);
#include "__string_encodings.def"
#undef _ENCODING
  }

  throw RuntimeException(U"Got invalid encoding in String::Encode()"s);
}

String String::Decode(Bytes const& bytes, Encoding encoding) {
  switch (encoding) {
#define _ENCODING(NAME)                                                                                                \
  case Encoding::NAME:                                                                                                 \
    return EncodingFunctions<Encoding::NAME>::Decode(bytes);
#include "__string_encodings.def"
#undef _ENCODING
  }

  throw RuntimeException(U"Got invalid encoding in String::Decode()"s);
}
