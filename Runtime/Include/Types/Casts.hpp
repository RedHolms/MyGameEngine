#pragma once

#include "Meta.hpp"

template <typename To, typename From>
  requires (sizeof(To) == sizeof(From)) && IsTriviallyCopyableType<To> && IsTriviallyCopyableType<From>
[[nodiscard]] constexpr To bit_cast(From const& value) noexcept {
  return __builtin_bit_cast(To, value);
}
