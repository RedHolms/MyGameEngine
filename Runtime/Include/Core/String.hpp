#pragma once
#include "Memory/Utils.hpp"

template <typename T>
constexpr bool IsCharType = IsAnyTypeOf<T, char, wchar_t, char8_t, char16_t, char32_t>;

template <typename T>
concept CharType = IsCharType<T>;

// Unicode string
class String {
public:
  using ItemType = char32_t;

public:
  class Iterator;
  class ConstIterator;

public:
  consteval String() noexcept = default;

  // NOTE: this is ASCII!!!
  constexpr String(const char* asciiString, size_t lengthInChars)
    : m_buffer(new char32_t[lengthInChars]),
      m_length(lengthInChars),
      m_capacity(lengthInChars)
  {
    for (size_t i = 0; i < m_length; ++i)
      m_buffer[i] = static_cast<char32_t>(asciiString[i]); // zero extend
  }

  constexpr String(const char32_t* string, size_t lengthInChars)
    : m_buffer(new char32_t[lengthInChars]),
      m_length(lengthInChars),
      m_capacity(lengthInChars)
  {
    Memory::CopyItems<char32_t>(m_buffer, string, lengthInChars);
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
  char32_t* m_buffer = nullptr; // non null-terminated
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
