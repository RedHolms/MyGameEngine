#pragma once

namespace TT /* type traits */ {

/**
 * IsSame / IsSameW / SameType
 * <typename T1, typename T2>
 *
 * Checks if 'T1' and 'T2' is the same type
 */

template <typename, typename>
constexpr bool IsSame = false;

template <typename T>
constexpr bool IsSame<T, T> = true;

template <typename T1, typename T2>
struct IsSameW { static constexpr bool Value = IsSame<T1, T2>; };

template <typename Target, typename T>
concept SameType = IsSame<Target, T>;

/**
 * IsAnyOf / IsAnyOfW / AnyTypeOf
 * <typename T, typename... Variants>
 *
 * Checks if 'T' is any of types listed in 'Variants'
 */

template <typename T, typename... Variants>
constexpr bool IsAnyOf = (IsSame<T, Variants> || ...);

template <typename T, typename... Variants>
struct IsAnyOfW { static constexpr bool Value = IsAnyOf<T, Variants...>; };

template <typename Target, typename... Types>
concept AnyTypeOf = IsAnyOf<Target, Types...>;

/**
 * IsIntegral / IsIntegralW / IntegralType
 * <typename T>
 *
 * Checks if 'T' is an integral type
 */

template <typename T>
constexpr bool IsIntegral = IsAnyOf<T,
  bool, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t, short, unsigned short, int,
  unsigned int, long, unsigned long, long long, unsigned long long
>;

template <typename T>
struct IsIntegralW { static constexpr bool Value = IsIntegral<T>; };

template <typename Target>
concept IntegralType = IsIntegral<Target>;

/**
 * IsBitwiseRelocatable / IsBitwiseRelocatableW / BitwiseRelocatableType
 * <typename T>
 *
 * Checks if 'T' can be safely copied/moved with 'Memory::Copy' and be forget after that.
 * I.e. no constructors/operators/destructors has to be called
 */

template <typename T>
constexpr bool IsBitwiseRelocatable = __is_trivially_copyable(T);

template <typename T>
struct IsBitwiseRelocatableW { static constexpr bool Value = IsBitwiseRelocatable<T>; };

template <typename Target>
concept BitwiseRelocatableType = IsBitwiseRelocatable<Target>;

} // namespace TT
