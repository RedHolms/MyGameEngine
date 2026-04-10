#pragma once

enum class Endianness {
  LITTLE,
  BIG,

  // Are there any big-endian architectures we can target?
  NATIVE = LITTLE
};

extern "C" uint16_t _byteswap_ushort(uint16_t);
#pragma intrinsic(_byteswap_ushort)
extern "C" uint32_t _byteswap_ulong(uint32_t);
#pragma intrinsic(_byteswap_ulong)
extern "C" uint64_t _byteswap_uint64(uint64_t);
#pragma intrinsic(_byteswap_uint64)

constexpr uint16_t _ByteSwap16(uint16_t v) noexcept {
  if (IsConstantEvaluated()) {
    // constexpr variant
    return static_cast<uint16_t>((v << 8) | (v >> 8));
  }
  else {
    // use intrinsic
    return _byteswap_ushort(v);
  }
}

constexpr uint32_t _ByteSwap32(uint32_t v) noexcept {
  if (IsConstantEvaluated()) {
    // constexpr variant
    return (v << 24) | ((v << 8) & 0x00FF'0000) | ((v >> 8) & 0x0000'FF00) | (v >> 24);
  }
  else {
    // use intrinsic
    return _byteswap_ulong(v);
  }
}

constexpr uint64_t _ByteSwap64(uint64_t v) noexcept {
  if (IsConstantEvaluated()) {
    // constexpr variant
    return (v << 56) | ((v << 40) & 0x00FF'0000'0000'0000) | ((v << 24) & 0x0000'FF00'0000'0000) |
           ((v << 8) & 0x0000'00FF'0000'0000) | ((v >> 8) & 0x0000'0000'FF00'0000) |
           ((v >> 24) & 0x0000'0000'00FF'0000) | ((v >> 40) & 0x0000'0000'0000'FF00) | (v >> 56);
  }
  else {
    // use intrinsic
    return _byteswap_uint64(v);
  }
}

template <IntegralType T>
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

template <IntegralType T>
constexpr T ConventEndianness(T value, Endianness from, Endianness to) noexcept {
  return from == to ? value : ByteSwap(value);
}

template <IntegralType T>
constexpr T ConventEndiannessFromNative(T value, Endianness to) noexcept {
  return ConventEndianness(value, Endianness::NATIVE, to);
}

template <IntegralType T>
constexpr T ConventEndiannessToNative(T value, Endianness from) noexcept {
  return ConventEndianness(value, from, Endianness::NATIVE);
}
