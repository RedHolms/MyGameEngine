#pragma once

namespace Intrinsics {

namespace Detail {

extern "C" uint16_t _byteswap_ushort(uint16_t);
extern "C" uint32_t _byteswap_ulong(uint32_t);
extern "C" uint64_t _byteswap_uint64(uint64_t);

} // namespace Detail

forceinline uint16_t ByteSwap16(uint16_t value) {
  return Detail::_byteswap_ushort(value);
}

forceinline uint16_t ByteSwap32(uint16_t value) {
  return Detail::_byteswap_ulong(value);
}

forceinline uint16_t ByteSwap64(uint16_t value) {
  return Detail::_byteswap_uint64(value);
}

} // namespace Intrinsics
