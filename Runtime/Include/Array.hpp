#pragma once

template <typename T, size_t N>
class Array {
public:
  T Items[N];

public:
  [[nodiscard]] constexpr T const& operator[](index_t index) const noexcept {
    return Items[_TranslateIndex(index)];
  }

  [[nodiscard]] constexpr T& operator[](index_t index) noexcept {
    return Items[_TranslateIndex(index)];
  }

private:
  constexpr size_t _TranslateIndex(index_t index) const noexcept {
    size_t realIndex = static_cast<size_t>(index < 0 ? N + index : index);
    if (realIndex >= N) UNLIKELY
      throw IndexOutOfRangeException(*this, index);
    return index;
  }
};
