#pragma once

#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/MemoryUtils.hpp>
#include <stdexcept>

class EncodedString;
class Bytes;

enum class Encoding : uint;

struct __StringViewTag { explicit constexpr __StringViewTag() = default; };
constexpr auto StringView = __StringViewTag();

/**
 * Unicode (UCS-4/UTF-32) string container
 */
class String final {
  friend constexpr String& operator*=(String& dest, size_t repeatTimes);
  friend consteval String operator""_s(const char32_t* string, size_t length) noexcept;

public:
  class Iterator;
  friend Iterator;

  class ConstIterator;
  friend ConstIterator;

public:
  consteval String() noexcept = default;

  /// Adopts the buffer without allocation. Buffer won't be modified. Instead, allocation will be performed on the next
  /// mutable operation.
  constexpr String(const char32_t* string, size_t lengthInChars, __StringViewTag) noexcept
    : m_buffer(const_cast<char32_t*>(string)),
      m_length(lengthInChars),
      m_capacity(0)
  {}

  constexpr String(const char32_t* string, size_t lengthInChars)
    : m_buffer(new char32_t[lengthInChars]),
      m_length(lengthInChars),
      m_capacity(lengthInChars)
  {
    MemoryUtils::Copy<char32_t>(m_buffer, string, lengthInChars);
  }

  constexpr ~String() noexcept {
    _Release();
  }

  constexpr String(String const& other) : m_length(other.m_length) {
    if (!other._IsBufferOwned()) {
      // We can just inherit the buffer
      m_buffer = other.m_buffer;
    }
    else if (m_length > 0) {
      // Clone the buffer
      m_buffer = new char32_t[m_length];
      m_capacity = m_length;
      MemoryUtils::Copy<char32_t>(m_buffer, other.m_buffer, m_length);
    }
  }

  constexpr String(String&& other) noexcept
    : m_buffer(other.m_buffer),
      m_length(other.m_length),
      m_capacity(other.m_capacity)
  {
    other.m_buffer = nullptr;
    other.m_length = 0;
    other.m_capacity = 0;
  }

  constexpr String& operator=(String const& other) {
    m_length = 0;

    if (!other.IsEmpty()) {
      _WantCapacity(other.m_length);
      m_length = other.m_length;
      MemoryUtils::Copy<char32_t>(m_buffer, other.m_buffer, m_length);
    }

    return *this;
  }

  constexpr String& operator=(String&& other) noexcept {
    _Release();
    m_buffer = other.m_buffer;
    m_length = other.m_length;
    m_capacity = other.m_capacity;
    other.m_buffer = nullptr;
    other.m_length = 0;
    other.m_capacity = 0;
    return *this;
  }

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

  [[nodiscard]] constexpr ConstIterator at(sindex_t signedIndex) const;
  [[nodiscard]] constexpr char32_t const& operator[](sindex_t signedIndex) const;

  // Returns a character at unsigned index without any checks
  [[nodiscard]] constexpr char32_t const& IndexUnsafe(index_t index) const noexcept {
    return m_buffer[index];
  }

  [[nodiscard]] constexpr int operator<=>(String const& other) const noexcept {
    if (m_buffer == other.m_buffer && m_length == other.m_length) [[unlikely]]
      return 0;

    if (IsEmpty())
      return other.IsEmpty() ? 0 : -1;
    if (other.IsEmpty())
      return 1;

    size_t minLen = m_length < other.m_length ? m_length : other.m_length;

    int result = 0;
    for (size_t i = 0; i < minLen; ++i) {
      auto a = m_buffer[i], b = other.m_buffer[i];
      if (a != b) {
        result = a < b ? -1 : +1;
        break;
      }
    }

    if (result != 0)
      return result;

    if (m_length < other.m_length)
      return -1;
    if (m_length > other.m_length)
      return 1;

    return 0;
  }

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

  [[nodiscard]] constexpr size_t Find(String const& substring, sindex_t signedStartFrom = 0) const noexcept {
    auto const substrLen = substring.m_length;

    index_t startFrom = _TranslateSignedIndex(signedStartFrom);

    if (substring.IsEmpty() || startFrom + substrLen > m_length) [[unlikely]]
      return -1;

    for (size_t i = startFrom; i < m_length - substrLen; ++i) {
      bool matches = true;

      for (size_t j = 0; j < substrLen; ++i) {
        if (m_buffer[i+j] != substring.m_buffer[j]) {
          matches = false;
          break;
        }
      }

      if (matches)
        return i;
    }

    return -1;
  }

  [[nodiscard]] constexpr bool Contains(String const& substring) const noexcept {
    return Find(substring) != -1;
  }

  [[nodiscard]] constexpr bool StartsWith(String const& substring) const noexcept {
    if (substring.IsEmpty()) [[unlikely]]
      return true;
    if (substring.m_length > m_length) [[unlikely]]
      return false;

    for (size_t i = 0; i < substring.m_length; ++i) {
      if (m_buffer[i] != substring.m_buffer[i])
        return false;
    }

    return true;
  }

  [[nodiscard]] constexpr bool EndsWith(String const& substring) const noexcept {
    if (substring.IsEmpty()) [[unlikely]]
      return true;
    if (substring.m_length > m_length) [[unlikely]]
      return false;

    auto const start = m_length - substring.m_length;
    for (size_t i = 0; i < substring.m_length; ++i) {
      if (m_buffer[start + i] != substring.m_buffer[i])
        return false;
    }

    return true;
  }

  [[nodiscard]] constexpr String Slice(sindex_t signedBegin, size_t count) const {
    if (IsEmpty() || count == 0) [[unlikely]]
      return String {};

    size_t begin = _TranslateSignedIndex(signedBegin);

    if (begin + count > m_length) [[unlikely]]
      count = m_length - begin;

    if (!_IsBufferOwned())
      return String { m_buffer + begin, count, StringView };
    else
      return String { m_buffer + begin, count };
  }

public:
  constexpr void Reserve(size_t newCapacity) {
    _WantCapacity(newCapacity);
  }

  constexpr void Clear() noexcept {
    m_length = 0;
  }

  constexpr void Append(char32_t codepoint) {
    _WantAddChars(1);
    m_buffer[m_length++] = codepoint;
  }

  constexpr void Append(String const& string) {
    if (string.IsEmpty()) [[unlikely]]
      return;

    _WantAddChars(string.m_length);
    MemoryUtils::Copy<char32_t>(m_buffer + m_length, string.m_buffer, string.m_length);
    m_length += string.m_length;
  }

  constexpr void Append(ConstIterator begin, ConstIterator end);

  constexpr void Insert(sindex_t signedAtIndex, char32_t codepoint) {
    size_t atIndex = _TranslateSignedIndex(signedAtIndex);

    if (atIndex == m_length) [[unlikely]]
      return Append(codepoint);

    _InsertCharRange(atIndex, &codepoint, 1);
  }

  constexpr void Insert(sindex_t signedAtIndex, String const& string) {
    if (string.IsEmpty())
      return;

    index_t atIndex = _TranslateSignedIndex(signedAtIndex);

    if (atIndex == m_length) [[unlikely]]
      return Append(string);

    _InsertCharRange(atIndex, string.m_buffer, string.m_length);
  }

  constexpr void Insert(sindex_t signedAtIndex, ConstIterator begin, ConstIterator end);

  constexpr void Remove(sindex_t signedAtIndex, size_t count = 1) noexcept {
    index_t atIndex = _TranslateSignedIndex(signedAtIndex);
    index_t end = atIndex + count;

    // Removing chars to the end of the string
    if (end >= m_length) {
      m_length = atIndex;
      return;
    }

    MemoryUtils::Copy<char32_t>(m_buffer + atIndex, m_buffer + end, count);
    m_length -= count;
  }

public:
  EncodedString Encode(Encoding encoding) const;
  static String Decode(Bytes const& bytes, Encoding encoding);

private:
  constexpr index_t _TranslateSignedIndex(sindex_t signedIndex) const;
  constexpr char32_t* _IteratorIndex(index_t index);
  constexpr char32_t const* _IteratorIndex(index_t index) const;

  constexpr bool _IsBufferOwned() const noexcept {
    return m_capacity != 0;
  }

  constexpr void _InsertCharRange(index_t atIndex, const char32_t* source, size_t count) {
    _WantAddChars(count);
    MemoryUtils::Copy<char32_t>(m_buffer + atIndex + count, m_buffer + atIndex, m_length - atIndex);
    MemoryUtils::Copy<char32_t>(m_buffer + atIndex, source, count);
    m_length += count;
  }

  // REQUIRES: repeatTimes > 0 && m_length > 0
  constexpr String& _OperatorRepeat(size_t repeatTimes) {
    _WantCapacity(m_length * repeatTimes);

    size_t origLen = m_length;
    m_length *= repeatTimes;

    for (size_t i = 1; i < repeatTimes; ++i)
      MemoryUtils::Copy<char32_t>(&m_buffer[i*origLen], m_buffer, origLen);

    return *this;
  }

  constexpr void _WantAddChars(size_t count) {
    _WantCapacity(m_length + count);
  }

  constexpr void _WantCapacity(size_t required) {
    if (m_capacity < required) [[unlikely]] {
      // Grow the buffer

      size_t newCapacity = m_capacity * 2;
      if (newCapacity < required)
        newCapacity = required;

      _Reallocate(newCapacity);
    }
  }

  constexpr void _Reallocate(size_t newCapacity) {
    auto newBuffer = new char32_t[newCapacity];

    if (m_buffer != nullptr) [[likely]] {
      // Copy previous string
      if (m_length > 0)
        MemoryUtils::Copy<char32_t>(newBuffer, m_buffer, m_length);

      // Delete previous buffer
      if (_IsBufferOwned())
        delete[] m_buffer;
    }

    m_buffer = newBuffer;
    m_capacity = newCapacity;
  }

  /// Destroy all resources held by the container
  constexpr void _Release() noexcept {
    if (m_buffer != nullptr && _IsBufferOwned()) [[likely]]
      delete[] m_buffer;
  }

private:
  char32_t* m_buffer = nullptr;
  size_t m_length = 0;
  size_t m_capacity = 0;
};

constexpr String& operator+=(String& dest, String const& right) {
  dest.Append(right);
  return dest;
}

constexpr String& operator+=(String& dest, char32_t right) {
  dest.Append(right);
  return dest;
}

constexpr String operator+(String const& left, String const& right) {
  String result = left;
  result += right;
  return result;
}

constexpr String operator+(String const& left, char32_t right) {
  String result = left;
  result += right;
  return result;
}

constexpr String& operator*=(String& dest, size_t repeatTimes) {
  if (repeatTimes == 0 || dest.IsEmpty()) [[unlikely]] {
    dest.Clear();
    return dest;
  }

  return dest._OperatorRepeat(repeatTimes);
}

constexpr String operator*(String const& string, size_t repeatTimes) {
  if (repeatTimes == 0 || string.IsEmpty()) [[unlikely]]
    return String {};

  String result;
  result.Reserve(string.GetLength() * repeatTimes);
  result = string;

  while (--repeatTimes > 0)
    result.Append(string);

  return result;
}

consteval String operator""_s(const char32_t* string, size_t length) noexcept {
  if (length == 0)
    return String {};
  return String(string, length, StringView);
}

enum class Encoding : uint {
#define _ENCODING(NAME, _UNIT_SIZE) NAME,
#define _ENCODING_ALIAS(NAME, ALIAS_TO) NAME = ALIAS_TO,
#include "__string_encodings.def"
#undef _ENCODING_ALIAS
#undef _ENCODING
};

constexpr size_t get_encoding_unit_size(Encoding encoding) noexcept {
  switch (encoding) {
#define _ENCODING(NAME, UNIT_SIZE) case Encoding::NAME: return UNIT_SIZE;
#define _ENCODING_ALIAS(_NAME, _ALIAS_TO)
#include "__string_encodings.def"
#undef _ENCODING_ALIAS
#undef _ENCODING
  default:
    // TODO: Unreachable
    return 4;
  }
}

template <char32_t... Chars>
struct __const_string {
  static constexpr char32_t c[] { Chars... };
};

namespace __make_string_const_details {

template <auto CharGetter, index_t... Indices>
consteval auto make2(std::index_sequence<Indices...>) noexcept {
  return __const_string<CharGetter(Indices)...> {};
}

template <auto CharGetter, size_t Length>
consteval String make() noexcept {
  constexpr auto str = make2<CharGetter>(std::make_integer_sequence<index_t, Length>{});
  return String { str.c, Length, StringView };
}

} // namespace __make_string_const_details

/**
 * Takes a const-expression that results with a @c String as an argument, produces a const array of char32_t with this
 * string and returns a @c String with @b const buffer (i.e. no run-time allocations)
 */
#define make_string_const(...)                                                                                         \
  ([&]{                                                                                                                \
    constexpr auto ___GetChar = [&](index_t i) -> char32_t { return (__VA_ARGS__).IndexUnsafe(i); };                   \
    return __make_string_const_details::make<___GetChar, (__VA_ARGS__).GetLength()>();                                 \
  }())

class String::Iterator final {
  friend String;

private:
  constexpr Iterator(String* container, index_t index) noexcept
    : m_container(container), m_index(index) {}

public:
  constexpr Iterator& operator++() noexcept {
    ++m_index;
    return *this;
  }

  constexpr Iterator operator++(int) noexcept {
    return ++Iterator(*this);
  }

  constexpr Iterator operator+=(sindex_t diff) noexcept {
    m_index += diff;
    return *this;
  }

  constexpr Iterator operator+(sindex_t diff) noexcept {
    return Iterator(*this) += diff;
  }

  constexpr Iterator& operator--() noexcept {
    --m_index;
    return *this;
  }

  constexpr Iterator operator--(int) noexcept {
    return --Iterator(*this);
  }

  constexpr Iterator operator-=(sindex_t diff) noexcept {
    m_index -= diff;
    return *this;
  }

  constexpr Iterator operator-(sindex_t diff) noexcept {
    return Iterator(*this) -= diff;
  }

  constexpr char32_t& operator*() const noexcept {
    return *m_container->_IteratorIndex(m_index);
  }

  constexpr operator char32_t*() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

  constexpr char32_t* operator->() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

  constexpr bool operator==(Iterator const& other) const noexcept {
    return m_container == other.m_container && m_index == other.m_index;
  }

  constexpr bool operator!=(Iterator const& other) const noexcept {
    return !operator==(other);
  }

  constexpr ptrdiff_t operator-(Iterator const& other) const noexcept {
    if (m_container != other.m_container)
      return -1;
    return static_cast<ptrdiff_t>(m_index) - other.m_index;
  }

private:
  String* const m_container;
  index_t m_index;
};

class String::ConstIterator final {
  friend String;

private:
  constexpr ConstIterator(String const* container, index_t index) noexcept
    : m_container(container), m_index(index) {}

public:
  constexpr ConstIterator& operator++() noexcept {
    ++m_index;
    return *this;
  }

  constexpr ConstIterator operator++(int) noexcept {
    return ++ConstIterator(*this);
  }

  constexpr ConstIterator operator+=(sindex_t diff) noexcept {
    m_index += diff;
    return *this;
  }

  constexpr ConstIterator operator+(sindex_t diff) noexcept {
    return ConstIterator(*this) += diff;
  }

  constexpr ConstIterator& operator--() noexcept {
    --m_index;
    return *this;
  }

  constexpr ConstIterator operator--(int) noexcept {
    return --ConstIterator(*this);
  }

  constexpr ConstIterator operator-=(sindex_t diff) noexcept {
    m_index -= diff;
    return *this;
  }

  constexpr ConstIterator operator-(sindex_t diff) noexcept {
    return ConstIterator(*this) -= diff;
  }

  constexpr char32_t const& operator*() const noexcept {
    return *m_container->_IteratorIndex(m_index);
  }

  constexpr operator char32_t const*() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

  constexpr char32_t const* operator->() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

  constexpr bool operator==(ConstIterator const& other) const noexcept {
    return m_container == other.m_container && m_index == other.m_index;
  }

  constexpr bool operator!=(ConstIterator const& other) const noexcept {
    return !operator==(other);
  }

  constexpr ptrdiff_t operator-(ConstIterator const& other) const noexcept {
    if (m_container != other.m_container)
      return -1;
    return static_cast<ptrdiff_t>(m_index) - other.m_index;
  }

private:
  String const* const m_container;
  index_t m_index;
};

constexpr String::Iterator String::begin() noexcept {
  return Iterator { this, 0 };
}

constexpr String::ConstIterator String::begin() const noexcept {
  return ConstIterator { this, 0 };
}

constexpr String::Iterator String::end() noexcept {
  return Iterator { this, m_length };
}

constexpr String::ConstIterator String::end() const noexcept {
  return ConstIterator { this, m_length };
}

constexpr String::ConstIterator String::at(sindex_t signedIndex) const {
  return ConstIterator { this, _TranslateSignedIndex(signedIndex) };
}

constexpr char32_t const& String::operator[](sindex_t signedIndex) const {
  return m_buffer[_TranslateSignedIndex(signedIndex)];
}

constexpr void String::Append(ConstIterator begin, ConstIterator end) {
  if (begin == end) [[unlikely]]
    return;

  size_t const count = end - begin;
  _WantAddChars(count);
  MemoryUtils::Copy<char32_t>(m_buffer + m_length, begin, count);
  m_length += count;
}

constexpr void String::Insert(sindex_t signedAtIndex, ConstIterator begin, ConstIterator end) {
  size_t atIndex = _TranslateSignedIndex(signedAtIndex);

  if (atIndex == m_length) [[unlikely]]
    return Append(begin, end);

  _InsertCharRange(atIndex, begin, end - begin);
}

constexpr index_t String::_TranslateSignedIndex(sindex_t signedIndex) const {
  size_t index = static_cast<size_t>(signedIndex < 0 ? m_length + signedIndex : signedIndex);
  if (index >= m_length) [[unlikely]]
    throw std::out_of_range("index in String is out of range");
  return index;
}

constexpr char32_t* String::_IteratorIndex(index_t index) {
  if (index >= m_length) [[unlikely]]
    throw std::out_of_range("iterator of String is out of range");
  return m_buffer + index;
}

constexpr char32_t const* String::_IteratorIndex(index_t index) const {
  if (index >= m_length) [[unlikely]]
    throw std::out_of_range("iterator of String is out of range");
  return m_buffer + index;
}
