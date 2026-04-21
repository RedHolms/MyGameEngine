#pragma once

#include "Intrinsics/ByteSwaps.hpp"
#include "Types/Traits.hpp"

enum class Endianness {
  LITTLE,
  BIG,

  // Are there any big-endian architectures we can target?
  NATIVE = LITTLE
};

constexpr uint16_t _ByteSwap16(uint16_t v) noexcept {
  if (IsConstantEvaluated()) {
    return static_cast<uint16_t>(
      (v << 8) |
      (v >> 8)
    );
  }
  else {
    return Intrinsics::ByteSwap16(v);
  }
}

constexpr uint32_t _ByteSwap32(uint32_t v) noexcept {
  if (IsConstantEvaluated()) {
    return static_cast<uint32_t>(
      (v << 24) |
      ((v << 8) & 0x00FF0000) |
      ((v >> 8) & 0x0000FF00) |
      (v >> 24)
    );
  }
  else {
    return Intrinsics::ByteSwap32(v);
  }
}

constexpr uint64_t _ByteSwap64(uint64_t v) noexcept {
  if (IsConstantEvaluated()) {
    return static_cast<uint64_t>(
      (v << 56) |
      ((v << 40) & 0x00FF000000000000) |
      ((v << 24) & 0x0000FF0000000000) |
      ((v <<  8) & 0x000000FF00000000) |
      ((v >>  8) & 0x00000000FF000000) |
      ((v >> 24) & 0x0000000000FF0000) |
      ((v >> 40) & 0x000000000000FF00) |
      (v >> 56)
    );
  }
  else {
    return Intrinsics::ByteSwap64(v);
  }
}

template <TT::IntegralType T>
constexpr T ByteSwap(T value) noexcept {
  if constexpr (sizeof(T) == 1) {
    return value;
  }
  else if constexpr (sizeof(T) == 2) {
    return static_cast<T>(_ByteSwap16(static_cast<uint16_t>(value)));
  }
  else if constexpr (sizeof(T) == 4) {
    return static_cast<T>(_ByteSwap32(static_cast<uint32_t>(value)));
  }
  else if constexpr (sizeof(T) == 8) {
    return static_cast<T>(_ByteSwap64(static_cast<uint64_t>(value)));
  }
  else {
    static_assert(false, "Invalid type passed to ByteSwap<>()");
    return value; // make analyzer happy
  }
}

template <TT::IntegralType T>
constexpr T ConventEndianness(T value, Endianness from, Endianness to) noexcept {
  return from == to ? value : ByteSwap(value);
}

template <TT::IntegralType T>
constexpr T ConventEndiannessFromNative(T value, Endianness to) noexcept {
  return ConventEndianness(value, Endianness::NATIVE, to);
}

template <TT::IntegralType T>
constexpr T ConventEndiannessToNative(T value, Endianness from) noexcept {
  return ConventEndianness(value, from, Endianness::NATIVE);
}
