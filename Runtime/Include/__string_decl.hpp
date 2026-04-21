#pragma once

class Buffer;
class Bytes;

enum class Encoding : uint;

struct String_ConstInit_Tag { explicit constexpr String_ConstInit_Tag() = default; };
constexpr auto String_ConstInit = String_ConstInit_Tag();

/**
 * Unicode (UCS-4/UTF-32) string container
 */
class String {
  friend constexpr String& operator*=(String& dest, size_t repeatTimes);
  friend consteval String operator""s(const char32_t* string, size_t length) noexcept;

public:
  class Iterator;
  friend Iterator;

  class ConstIterator;
  friend ConstIterator;

public:
  // FIXME: Should be marked consteval but MSVC crashes for some reason if we do so
  constexpr String() noexcept;
  constexpr String(const char32_t* string, size_t lengthInChars);
  constexpr String(const char32_t* string, size_t lengthInChars, String_ConstInit_Tag) noexcept;
  constexpr ~String() noexcept;

  constexpr String(String const& other);
  constexpr String(String&& other) noexcept;
  constexpr String& operator=(String const& other);
  constexpr String& operator=(String&& other) noexcept;

public:
  [[nodiscard]] constexpr size_t GetLength() const noexcept;

  [[nodiscard]] constexpr bool IsEmpty() const noexcept;

  [[nodiscard]] constexpr size_t GetCapacity() const noexcept;

  [[nodiscard]] constexpr Iterator begin() noexcept;
  [[nodiscard]] constexpr ConstIterator begin() const noexcept;
  [[nodiscard]] constexpr Iterator end() noexcept;
  [[nodiscard]] constexpr ConstIterator end() const noexcept;

  // FIXME: no mutable versions of at() and operator[] because buffer can be not owned
  //  we COULD allocate real buffer, but doing allocation on simple indexing seems trash to me

  [[nodiscard]] constexpr ConstIterator at(index_t index) const;
  [[nodiscard]] constexpr char32_t const& operator[](index_t index) const;

  [[nodiscard]] constexpr int operator<=>(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator==(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator!=(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator<(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator>(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator<=(String const& other) const noexcept;
  [[nodiscard]] constexpr bool operator>=(String const& other) const noexcept;

  [[nodiscard]] constexpr size_t Find(String const& substring, index_t startFrom = 0) const noexcept;

  [[nodiscard]] constexpr bool Contains(String const& substring) const noexcept;
  [[nodiscard]] constexpr bool StartsWith(String const& substring) const noexcept;
  [[nodiscard]] constexpr bool EndsWith(String const& substring) const noexcept;

  [[nodiscard]] constexpr String Slice(index_t begin, size_t count) const;

public:
  constexpr void Reserve(size_t newCapacity);
  constexpr void Clear() noexcept;

  constexpr void Append(char32_t codepoint);
  constexpr void Append(String const& string);

  constexpr void Insert(index_t atIndex, char32_t codepoint);
  constexpr void Insert(index_t atIndex, String const& string);

  constexpr void Remove(index_t atIndex, size_t count = 1) noexcept;

public:
  Buffer Encode(Encoding encoding) const;
  static String Decode(Bytes const& bytes, Encoding encoding);

private:
  template <typename T>
  static constexpr size_t _CountNullTerminatedStringLength(const T* string) noexcept;

  constexpr size_t _TranslateIndex(index_t index) const;
  constexpr char32_t* _IteratorIndex(size_t);
  constexpr char32_t const* _IteratorIndex(size_t) const;

  constexpr bool _IsBufferOwned() const noexcept;

  constexpr void _Reset();

  constexpr void _InsertCharRange(size_t atIndex, const char32_t* source, size_t count);

  // ASSERT: repeatTimes > 0 && m_length > 0
  constexpr String& _OperatorRepeat(size_t repeatTimes);

  constexpr void _WantAddChars(size_t count);
  constexpr void _WantCapacity(size_t required);
  constexpr void _Reallocate(size_t newCapacity);

private:
  char32_t* m_buffer = nullptr;
  size_t m_length = 0;
  size_t m_capacity = 0;
};

constexpr String& operator+=(String& dest, String const& right);
constexpr String& operator+=(String& dest, char32_t right);
constexpr String operator+(String const& left, String const& right);
constexpr String operator+(String const& left, char32_t right);
constexpr String& operator*=(String& dest, size_t repeatTimes);
constexpr String operator*(String const& string, size_t repeatTimes);

constexpr String::String() noexcept = default;

constexpr String::String(const char32_t* string, size_t lengthInChars, String_ConstInit_Tag) noexcept
  : m_buffer(const_cast<char32_t*>(string)),
    m_length(lengthInChars),
    m_capacity(0)
{}

consteval String operator""s(const char32_t* string, size_t length) noexcept {
  if (length == 0)
    return String {};
  return String(string, length, String_ConstInit);
}

enum class Encoding : uint {
#define _ENCODING(NAME) NAME,
#include "__string_encodings.def"
#undef _ENCODING
};
