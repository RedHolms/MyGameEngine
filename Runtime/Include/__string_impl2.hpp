#pragma once

class String::Iterator final : public BasicIterator<String, char32_t> {
  using BasicIterator::BasicIterator;
};

class String::ConstIterator final : public BasicConstIterator<String, char32_t> {
  using BasicConstIterator::BasicConstIterator;
};

[[nodiscard]] constexpr String::Iterator String::begin() noexcept {
  return Iterator { this, 0 };
}

[[nodiscard]] constexpr String::ConstIterator String::begin() const noexcept {
  return ConstIterator { this, 0 };
}

[[nodiscard]] constexpr String::Iterator String::end() noexcept {
  return Iterator { this, m_length };
}

[[nodiscard]] constexpr String::ConstIterator String::end() const noexcept {
  return ConstIterator { this, m_length };
}

[[nodiscard]] constexpr String::ConstIterator String::at(index_t index) const {
  return ConstIterator { this, _TranslateIndex(index) };
}

constexpr size_t String::_TranslateIndex(index_t index) const {
  size_t realIndex = static_cast<size_t>(index < 0 ? m_length + index : index);
  if (realIndex >= m_length) UNLIKELY
    throw IndexOutOfRangeException(*this, index);
  return index;
}

constexpr char32_t* String::_IteratorIndex(size_t index) {
  if (index >= m_length) UNLIKELY
    throw IteratorOutOfRangeException(*this, index);
  return m_buffer + index;
}

constexpr char32_t const* String::_IteratorIndex(size_t index) const {
  if (index >= m_length) UNLIKELY
    throw IteratorOutOfRangeException(*this, index);
  return m_buffer + index;
}
