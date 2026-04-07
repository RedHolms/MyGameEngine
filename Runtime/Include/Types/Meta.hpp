#pragma once

template <class Wrapper>
using UnwrapType = Wrapper::Type;

template <typename...>
using AsVoid = void;

template <typename T>
struct TypeIdentity { using Type = T; };

template <typename T>
using ExplicitType = TypeIdentity<T>::Type;

template <typename T>
struct __RemoveReference_WRAPPER {
  using Type = T;
};

template <typename T>
struct __RemoveReference_WRAPPER<T&> {
  using Type = T;
};

template <typename T>
struct __RemoveReference_WRAPPER<T&&> {
  using Type = T;
};

template <typename T>
using RemoveReference = UnwrapType<__RemoveReference_WRAPPER<T>>;

template <typename T, typename = void>
struct __TryAddReferences { // non-referenceable type
  using _Lvalue = T;
  using _Rvalue = T;
};

template <typename T>
struct __TryAddReferences<T, AsVoid<T&>> { // referenceable type
  using _Lvalue = T&;
  using _Rvalue = T&&;
};

template <typename T>
using TryAddLValueReference = __TryAddReferences<T>::_Lvalue;

template <typename T>
using TryAddRValueReference = __TryAddReferences<T>::_Rvalue;

template <typename T>
constexpr bool IsLValueReference = false;

template <typename T>
constexpr bool IsLValueReference<T&> = true;

template <typename T>
[[nodiscard]] constexpr T&& ForwardArg(RemoveReference<T>& arg) noexcept {
  return static_cast<T&&>(arg);
}

template <typename T>
[[nodiscard]] constexpr T&& ForwardArg(RemoveReference<T>&& arg) noexcept {
  static_assert(!IsLValueReference<T>, "bad ForwardArg<> call");
  return static_cast<T&&>(arg);
}

template <typename T>
[[nodiscard]] constexpr RemoveReference<T>&& Move(T&& _Arg) noexcept {
  return static_cast<RemoveReference<T>&&>(_Arg);
}

template <typename T>
constexpr bool IsTriviallyMoveConstructibleType = __is_trivially_constructible(T, T);

template <typename T>
constexpr bool IsTriviallyMoveAssignableType = __is_trivially_assignable(TryAddLValueReference<T>, T);

template <typename T>
constexpr bool IsTriviallyCopyConstructibleType = __is_trivially_constructible(T, TryAddLValueReference<T const>);

template <typename T>
constexpr bool IsTriviallyCopyAssignableType =
  __is_trivially_assignable(TryAddLValueReference<T>, TryAddLValueReference<T const>);

template <typename T>
constexpr bool IsTriviallyCopyableType = __is_trivially_copyable(T);

template <typename T>
constexpr bool IsTriviallyDestructibleType = __is_trivially_destructible(T);
