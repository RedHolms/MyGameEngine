#pragma once

[[nodiscard]] constexpr bool IsConstantEvaluated() noexcept {
  return __builtin_is_constant_evaluated();
}

template <typename T, size_t N>
[[nodiscard]] consteval size_t CountOf(T (&)[N]) noexcept {
  return N;
}
