#pragma once

#include <MyGameEngine/Assert.hpp>
#include <MyGameEngine/Bytes.hpp>
#include <MyGameEngine/String.hpp>

// Represents an encoded string. String can contain multiple null units in it, but also must ALWAYS terminate with a
// null unit.
class EncodedString final {
public:
  // Creates empty encoded string
  constexpr explicit EncodedString(Encoding encoding)
    : m_encoding(encoding),
      m_lengthInBytes(0)
  {}

  // Steals 'buffer'. 'lengthInBytes' must be length of string EXCLUDING last null unit. Length must be multiple of
  // encoding units (i.e. for UTF32 it must be multiple of 4). Buffer MUST contain a null unit at offset
  // 'lengthInBytes'. Thus buffer size must be at least 'lengthInBytes + <ENCODING UNIT SIZE>'.
  constexpr EncodedString(Encoding encoding, Buffer&& buffer, size_t lengthInBytes)
    : m_encoding(encoding),
      m_buffer(std::move(buffer)),
      m_lengthInBytes(lengthInBytes)
  {
    auto unitSize = GetEncodingUnitSize();

    MGE_ASSERT((lengthInBytes % unitSize) == 0, "Buffer size is not multiple of encoding units");
    MGE_ASSERT(lengthInBytes <= m_buffer.GetSize()-unitSize, "Buffer is too small for the string and null terminator");

    bool hasNullTerm = true;
    switch (unitSize) {
      case 1:
        hasNullTerm = m_buffer.Read<char>(lengthInBytes) == 0;
        break;
      case 2:
        hasNullTerm = m_buffer.Read<short>(lengthInBytes) == 0;
        break;
      case 4:
        hasNullTerm = m_buffer.Read<int>(lengthInBytes) == 0;
        break;
    }

    MGE_ASSERT(hasNullTerm, "String isn't null terminated");
  }

public:
  // Can return nullptr
  template <typename T = void>
  inline const T* GetCString() const noexcept {
    return static_cast<const T*>(m_buffer.GetBuffer());
  }

  inline size_t GetLengthInBytes() const noexcept {
    return m_lengthInBytes;
  }

  inline size_t GetLengthInUnits() const noexcept {
    return m_lengthInBytes / GetEncodingUnitSize();
  }

  inline Encoding GetEncoding() const noexcept {
    return m_encoding;
  }

  inline size_t GetEncodingUnitSize() const noexcept {
    return get_encoding_unit_size(m_encoding);
  }

private:
  Encoding m_encoding; // In what encoding string is encoded
  Buffer m_buffer; // Buffer containing encoded string
  size_t m_lengthInBytes; // Encoded string length in bytes (excluding last null terminator)
};
