#pragma once

[[nodiscard]] constexpr bool IsConstantEvaluated() noexcept {
  return __builtin_is_constant_evaluated();
}

// Use "declval<T>" to create an expression that results with "T"
template <typename T>
constexpr T declval;

template <class T>
[[nodiscard]] constexpr size_t GetContainerSize(T const& container) noexcept {
  if constexpr (requires { container.GetSize(); })
    return container.GetSize();
  else if constexpr (requires { container.GetCount(); })
    return container.GetCount();
  else if constexpr (requires { container.GetLength(); })
    return container.GetLength();
  else {
    static_assert(false, "Don't know how to get size of this container");
    return -1;
  }
}
