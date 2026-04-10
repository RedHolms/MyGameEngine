#pragma once

#include "Memory/Buffer.hpp"
#include "Memory/Utils.hpp"
#include "Types/Chars.hpp"

enum class Encoding : uint;

/**
 * Unicode (UCS-4) string container
 */
class String {
public:
  using ItemType = ucs4_t;

public:
  class Iterator;
  class ConstIterator;

public:
  consteval String() noexcept = default;

  // NOTE: this is ASCII!!!
  constexpr String(const char* asciiString, size_t lengthInChars) noexcept
    : m_buffer(new ucs4_t[lengthInChars]),
      m_length(lengthInChars),
      m_capacity(lengthInChars)
  {
    for (size_t i = 0; i < m_length; ++i)
      m_buffer[i] = static_cast<ucs4_t>(asciiString[i]); // zero extend
  }

  constexpr String(const char32_t* string, size_t lengthInChars) noexcept
    : m_buffer(new ucs4_t[lengthInChars]),
      m_length(lengthInChars),
      m_capacity(lengthInChars)
  {
    static_assert(sizeof(char32_t) == sizeof(ucs4_t));

    // UTF-32 and UCS-4 are effectively the same
    if (IsConstantEvaluated()) {
      for (size_t i = 0; i < m_length; ++i)
        m_buffer[i] = static_cast<ucs4_t>(string[i]);
    }
    else {
      Memory::CopyBytes(m_buffer, string, lengthInChars * sizeof(char32_t));
    }
  }

  constexpr ~String() noexcept {
    _Reset();
  }

  constexpr String(String const&) noexcept;
  constexpr String(String&&) noexcept;
  constexpr String& operator=(String const&) noexcept;
  constexpr String& operator=(String&&) noexcept;

public:
  [[nodiscard]] constexpr size_t GetLength() const noexcept {
    return m_length;
  }

  [[nodiscard]] constexpr bool IsEmpty() const noexcept {
    return m_length == 0;
  }

  [[nodiscard]] constexpr size_t GetCapacity() const noexcept {
    return m_capacity;
  }

  [[nodiscard]] constexpr Iterator begin() noexcept;
  [[nodiscard]] constexpr ConstIterator begin() const noexcept;
  [[nodiscard]] constexpr Iterator end() noexcept;
  [[nodiscard]] constexpr ConstIterator end() const noexcept;
  [[nodiscard]] constexpr Iterator at(index_t index) noexcept;
  [[nodiscard]] constexpr ConstIterator at(index_t index) const noexcept;

  [[nodiscard]] constexpr char32_t& operator[](index_t index) noexcept;
  [[nodiscard]] constexpr char32_t const& operator[](index_t index) const noexcept;

  [[nodiscard]] constexpr int operator<=>(String const& other) const noexcept;

  [[nodiscard]] constexpr bool operator==(String const& other) const noexcept {
    return (*this <=> other) == 0;
  }

  [[nodiscard]] constexpr bool operator!=(String const& other) const noexcept {
    return (*this <=> other) != 0;
  }

  [[nodiscard]] constexpr bool operator<(String const& other) const noexcept {
    return (*this <=> other) == -1;
  }

  [[nodiscard]] constexpr bool operator>(String const& other) const noexcept {
    return (*this <=> other) == 1;
  }

  [[nodiscard]] constexpr bool operator<=(String const& other) const noexcept {
    return (*this <=> other) != 1;
  }

  [[nodiscard]] constexpr bool operator>=(String const& other) const noexcept {
    return (*this <=> other) != -1;
  }

  [[nodiscard]] constexpr size_t Find(String const& substring, index_t startFrom = 0) const noexcept;
  [[nodiscard]] constexpr bool Contains(String const& substring) const noexcept;
  [[nodiscard]] constexpr bool StartsWith(String const& substring) const noexcept;
  [[nodiscard]] constexpr bool EndsWith(String const& substring) const noexcept;

  [[nodiscard]] constexpr String Slice(index_t begin, size_t count) const noexcept;

public:
  constexpr void Reserve(size_t newCapacity) noexcept;
  constexpr void Clear() noexcept;

  constexpr void Append(String const& string) noexcept;
  constexpr void Insert(index_t atIndex, String const& string) noexcept;
  constexpr void Remove(index_t atIndex, size_t count = 1) noexcept;

public:
  Buffer Encode(Encoding encoding) const noexcept;

  // TODO: Create "BufferView"-like class to accept any bytes span
  static String Decode(Buffer const& bytes, Encoding encoding) noexcept;

private:
  template <typename T>
  static constexpr size_t _CountNullTerminatedStringLength(const T* string) noexcept {
    size_t length = 0;
    while (string[length])
      ++length;
    return length;
  }

  constexpr size_t _TranslateIndex(index_t index) const noexcept {
    if (index < 0)
      return m_length + index;
    return index;
  }

  constexpr void _WantAddChars(size_t count) noexcept {
    _WantCapacity(m_length + count);
  }

  constexpr void _Reset() {
    delete[] m_buffer;
  }

  constexpr void _WantCapacity(size_t required) noexcept {}

  constexpr void _Reallocate(size_t newCapacity) noexcept {}

private:
  ucs4_t* m_buffer = nullptr; // non null-terminated
  size_t m_length = 0;
  size_t m_capacity = 0;
};

constexpr String operator+(String const& left, String const& right) noexcept;
constexpr String& operator+=(String& dest, String const& right) noexcept;
constexpr String operator*(String const& string, size_t repeatTimes) noexcept;
constexpr String& operator*=(String& dest, size_t repeatTimes) noexcept;

// ASCII
constexpr String operator""s(const char* asciiString, size_t length) noexcept {
  return String(asciiString, length);
}

constexpr String operator""s(const char32_t* string, size_t length) noexcept {
  return String(string, length);
}

enum class Encoding : uint {
  ASCII,
  WCHAR, // Platform-dependent "wchar_t" encoding

  UTF8,
  UTF16, // native endianness
  UTF16_BE,
  UTF16_LE,
  UTF32, // native endianness
  UTF32_BE,
  UTF32_LE,

  // ANSI code pages
  CP1251,
#if IS_WINDOWS_OS
  CP_ACP, // Active system code page
#endif
};
