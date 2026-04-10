#pragma once

template <typename T>
struct WrapType {
  using Type = T;
};

template <class Wrapper>
using UnwrapType = Wrapper::Type;

template <auto V>
struct WrapValue {
  static constexpr auto Value = V;
};

template <class Wrapper>
constexpr auto UnwrapValue = Wrapper::Value;

template <bool Select, typename TrueType, typename FalseType>
struct __ConditionalType_WRAPPER;

template <typename T, typename F>
struct __ConditionalType_WRAPPER<true, T, F> : WrapType<T> {};

template <typename T, typename F>
struct __ConditionalType_WRAPPER<false, T, F> : WrapType<F> {};

template <bool Select, typename TrueType, typename FalseType>
using ConditionalType = UnwrapType<__ConditionalType_WRAPPER<Select, TrueType, FalseType>>;

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
concept TriviallyCopyableType = IsTriviallyCopyableType<T>;

template <typename T>
constexpr bool IsTriviallyDestructibleType = __is_trivially_destructible(T);

template <typename, typename>
constexpr bool IsSameType = false;

template <typename T>
constexpr bool IsSameType<T, T> = true;

template <typename T, typename... Ts>
constexpr bool IsAnyTypeOf = (IsSameType<T, Ts> || ...);

template <typename T, typename... Ts>
concept AnyTypeOf = IsAnyTypeOf<T, Ts...>;

template <typename T>
constexpr bool IsIntegralType = IsAnyTypeOf<T,
  bool, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t, short, unsigned short, int,
  unsigned int, long, unsigned long, long long, unsigned long long
>;

template <typename T>
concept IntegralType = IsIntegralType<T>;
