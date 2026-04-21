#pragma once

constexpr String::String(const char32_t* string, size_t lengthInChars)
  : m_buffer(new char32_t[lengthInChars]),
    m_length(lengthInChars),
    m_capacity(lengthInChars)
{
  Memory::Copy<char32_t>(m_buffer, string, lengthInChars);
}

constexpr String::~String() noexcept {
  _Reset();
}

constexpr String::String(String const& other)
  : m_buffer(other.m_length > 0 ? new char32_t[other.m_length] : nullptr),
    m_length(other.m_length),
    m_capacity(other.m_length)
{
  if (m_buffer != nullptr)
    Memory::Copy<char32_t>(m_buffer, other.m_buffer, m_length);
}

constexpr String::String(String&& other) noexcept
  : m_buffer(other.m_buffer),
    m_length(other.m_length),
    m_capacity(other.m_capacity)
{
  other.m_buffer = nullptr;
  other.m_length = 0;
  other.m_capacity = 0;
}

constexpr String& String::operator=(String const& other) {
  m_length = 0;

  if (!other.IsEmpty()) {
    _WantCapacity(other.m_length);
    m_length = other.m_length;
    Memory::Copy<char32_t>(m_buffer, other.m_buffer, m_length);
  }

  return *this;
}

constexpr String& String::operator=(String&& other) noexcept {
  _Reset();
  m_buffer = other.m_buffer;
  m_length = other.m_length;
  m_capacity = other.m_capacity;
  other.m_buffer = nullptr;
  other.m_length = 0;
  other.m_capacity = 0;
  return *this;
}

[[nodiscard]] constexpr size_t String::GetLength() const noexcept {
  return m_length;
}

[[nodiscard]] constexpr bool String::IsEmpty() const noexcept {
  return m_length == 0;
}

[[nodiscard]] constexpr size_t String::GetCapacity() const noexcept {
  return m_capacity;
}

[[nodiscard]] constexpr char32_t const& String::operator[](index_t index) const {
  return m_buffer[_TranslateIndex(index)];
}

[[nodiscard]] constexpr int String::operator<=>(String const& other) const noexcept {
  if (m_buffer == other.m_buffer) UNLIKELY
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

[[nodiscard]] constexpr bool String::operator==(String const& other) const noexcept {
  return (*this <=> other) == 0;
}

[[nodiscard]] constexpr bool String::operator!=(String const& other) const noexcept {
  return (*this <=> other) != 0;
}

[[nodiscard]] constexpr bool String::operator<(String const& other) const noexcept {
  return (*this <=> other) == -1;
}

[[nodiscard]] constexpr bool String::operator>(String const& other) const noexcept {
  return (*this <=> other) == 1;
}

[[nodiscard]] constexpr bool String::operator<=(String const& other) const noexcept {
  return (*this <=> other) != 1;
}

[[nodiscard]] constexpr bool String::operator>=(String const& other) const noexcept {
  return (*this <=> other) != -1;
}

[[nodiscard]] constexpr size_t String::Find(String const& substring, index_t startFrom) const noexcept {
  auto const substrLen = substring.m_length;

  if (substring.IsEmpty() || startFrom + substrLen > m_length) UNLIKELY
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

[[nodiscard]] constexpr bool String::Contains(String const& substring) const noexcept {
  return Find(substring) != -1;
}

[[nodiscard]] constexpr bool String::StartsWith(String const& substring) const noexcept {
  if (substring.IsEmpty()) UNLIKELY
    return true;
  if (substring.m_length > m_length) UNLIKELY
    return false;

  for (size_t i = 0; i < substring.m_length; ++i) {
    if (m_buffer[i] != substring.m_buffer[i])
      return false;
  }

  return true;
}

[[nodiscard]] constexpr bool String::EndsWith(String const& substring) const noexcept {
  if (substring.IsEmpty()) UNLIKELY
    return true;
  if (substring.m_length > m_length) UNLIKELY
    return false;

  auto const start = m_length - substring.m_length;
  for (size_t i = 0; i < substring.m_length; ++i) {
    if (m_buffer[start + i] != substring.m_buffer[i])
      return false;
  }

  return true;
}

[[nodiscard]] constexpr String String::Slice(index_t begin, size_t count) const {
  if (IsEmpty() || count == 0) UNLIKELY
    return String {};

  size_t realBegin = _TranslateIndex(begin);

  if (realBegin + count > m_length) UNLIKELY
    count = m_length - realBegin;

  if (!_IsBufferOwned())
    return String { m_buffer + realBegin, count, String_ConstInit };
  else
    return String { m_buffer + realBegin, count };
}

constexpr void String::Reserve(size_t newCapacity) {
  _WantCapacity(newCapacity);
}

constexpr void String::Clear() noexcept {
  m_length = 0;
}

constexpr void String::Append(char32_t codepoint) {
  _WantAddChars(1);
  m_buffer[m_length++] = codepoint;
}

constexpr void String::Append(String const& string) {
  if (string.IsEmpty()) UNLIKELY
    return;

  _WantAddChars(string.m_length);
  Memory::Copy<char32_t>(m_buffer + m_length, string.m_buffer, string.m_length);
  m_length += string.m_length;
}

constexpr void String::Insert(index_t atIndex, char32_t codepoint) {
  size_t realAtIndex = _TranslateIndex(atIndex);

  if (realAtIndex == m_length) UNLIKELY
    return Append(codepoint);

  _InsertCharRange(realAtIndex, &codepoint, 1);
}

constexpr void String::Insert(index_t atIndex, String const& string) {
  if (string.IsEmpty())
    return;

  size_t realAtIndex = _TranslateIndex(atIndex);

  if (realAtIndex == m_length) UNLIKELY
    return Append(string);

  _InsertCharRange(realAtIndex, string.m_buffer, string.m_length);
}

constexpr void String::Remove(index_t atIndex, size_t count) noexcept {
  size_t realAtIndex = _TranslateIndex(atIndex);
  size_t end = realAtIndex + count;

  // Removing chars to the end of the string
  if (end >= m_length) {
    m_length = realAtIndex;
    return;
  }

  Memory::Copy<char32_t>(m_buffer + realAtIndex, m_buffer + end, count);
  m_length -= count;
}

template <typename T>
constexpr size_t String::_CountNullTerminatedStringLength(const T* string) noexcept {
  size_t length = 0;
  while (string[length])
    ++length;
  return length;
}

constexpr bool String::_IsBufferOwned() const noexcept {
  return m_capacity != 0;
}

constexpr void String::_Reset() {
  if (m_buffer != nullptr && _IsBufferOwned()) LIKELY
    delete[] m_buffer;
}

constexpr void String::_InsertCharRange(size_t atIndex, const char32_t* source, size_t count) {
  _WantAddChars(count);
  Memory::Copy<char32_t>(m_buffer + atIndex + count, m_buffer + atIndex, m_length - atIndex);
  Memory::Copy<char32_t>(m_buffer + atIndex, source, count);
  m_length += count;
}

// ASSERT: repeatTimes > 0 && m_length > 0
constexpr String& String::_OperatorRepeat(size_t const repeatTimes) {
  _WantCapacity(m_length * repeatTimes);

  size_t origLen = m_length;
  m_length *= repeatTimes;

  for (size_t i = 1; i < repeatTimes; ++i)
    Memory::Copy<char32_t>(&m_buffer[i*origLen], m_buffer, origLen);

  return *this;
}

constexpr void String::_WantAddChars(size_t count) {
  _WantCapacity(m_length + count);
}

constexpr void String::_WantCapacity(size_t required) {
  if (m_capacity < required) UNLIKELY {
    size_t newCapacity = m_capacity * 2;
    if (newCapacity < required)
      newCapacity = required;
    _Reallocate(newCapacity);
  }
}

constexpr void String::_Reallocate(size_t newCapacity) {
  auto newBuffer = new char32_t[newCapacity];

  if (m_buffer != nullptr && _IsBufferOwned()) LIKELY {
    if (m_length > 0)
      Memory::Copy<char32_t>(newBuffer, m_buffer, m_length);
    delete[] m_buffer;
  }

  m_buffer = newBuffer;
  m_capacity = newCapacity;
}

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
  if (repeatTimes == 0 || dest.IsEmpty()) {
    dest.Clear();
    return dest;
  }

  return dest._OperatorRepeat(repeatTimes);
}

constexpr String operator*(String const& string, size_t repeatTimes) {
  if (repeatTimes == 0 || string.IsEmpty())
    return String{};

  String result;
  result.Reserve(string.GetLength() * repeatTimes);
  result = string;

  // Got 1 copy in "result" already
  while (repeatTimes-- > 0)
    result.Append(string);

  return result;
}
