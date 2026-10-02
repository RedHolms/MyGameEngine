#pragma once

#include <MyGameEngine/IntTypes.hpp>
#include <MyGameEngine/MemoryUtils.hpp>
#include <bit>
#include <stdexcept>

enum class Endianness {
  LITTLE,
  BIG,

  // Are there any big-endian architectures we can target?
  NATIVE = LITTLE
};

template <typename T>
constexpr T ConventEndianness(T value, Endianness from, Endianness to) noexcept {
  return from == to ? value : std::byteswap(value);
}

template <typename T>
constexpr T ConventEndiannessFromNative(T value, Endianness to) noexcept {
  return ConventEndianness(value, Endianness::NATIVE, to);
}

template <typename T>
constexpr T ConventEndiannessToNative(T value, Endianness from) noexcept {
  return ConventEndianness(value, from, Endianness::NATIVE);
}

#define __BYTES_READ_FUNCTIONS                                                                                         \
  inline int8 ReadInt8(size_t offset) const {                                                                          \
    return Read<int8>(offset);                                                                                         \
  }                                                                                                                    \
                                                                                                                       \
  inline int16 ReadInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {                            \
    return ConventEndiannessToNative(Read<int16>(offset), endianness);                                                 \
  }                                                                                                                    \
                                                                                                                       \
  inline int32 ReadInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {                            \
    return ConventEndiannessToNative(Read<int32>(offset), endianness);                                                 \
  }                                                                                                                    \
                                                                                                                       \
  inline int64 ReadInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {                            \
    return ConventEndiannessToNative(Read<int64>(offset), endianness);                                                 \
  }                                                                                                                    \
                                                                                                                       \
  inline uint8 ReadUInt8(size_t offset) const {                                                                        \
    return Read<uint8>(offset);                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline uint16 ReadUInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {                          \
    return ConventEndiannessToNative(Read<uint16>(offset), endianness);                                                \
  }                                                                                                                    \
                                                                                                                       \
  inline uint32 ReadUInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {                          \
    return ConventEndiannessToNative(Read<uint32>(offset), endianness);                                                \
  }                                                                                                                    \
                                                                                                                       \
  inline uint64 ReadUInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {                          \
    return ConventEndiannessToNative(Read<uint64>(offset), endianness);                                                \
  }                                                                                                                    \
                                                                                                                       \
  inline float ReadFloat(size_t offset, Endianness endianness = Endianness::NATIVE) const {                            \
    return std::bit_cast<float>(ReadUInt32(offset, endianness));                                                       \
  }                                                                                                                    \
                                                                                                                       \
  inline double ReadDouble(size_t offset, Endianness endianness = Endianness::NATIVE) const {                          \
    return std::bit_cast<double>(ReadUInt64(offset, endianness));                                                      \
  }                                                                                                                    \
                                                                                                                       \
  inline int16 ReadInt16BE(size_t offset) const { return ReadInt16(offset, Endianness::BIG); }                         \
  inline int16 ReadInt16LE(size_t offset) const { return ReadInt16(offset, Endianness::LITTLE); }                      \
  inline int32 ReadInt32BE(size_t offset) const { return ReadInt32(offset, Endianness::BIG); }                         \
  inline int32 ReadInt32LE(size_t offset) const { return ReadInt32(offset, Endianness::LITTLE); }                      \
  inline int64 ReadInt64BE(size_t offset) const { return ReadInt64(offset, Endianness::BIG); }                         \
  inline int64 ReadInt64LE(size_t offset) const { return ReadInt64(offset, Endianness::LITTLE); }                      \
                                                                                                                       \
  inline uint16 ReadUInt16BE(size_t offset) const { return ReadUInt16(offset, Endianness::BIG); }                      \
  inline uint16 ReadUInt16LE(size_t offset) const { return ReadUInt16(offset, Endianness::LITTLE); }                   \
  inline uint32 ReadUInt32BE(size_t offset) const { return ReadUInt32(offset, Endianness::BIG); }                      \
  inline uint32 ReadUInt32LE(size_t offset) const { return ReadUInt32(offset, Endianness::LITTLE); }                   \
  inline uint64 ReadUInt64BE(size_t offset) const { return ReadUInt64(offset, Endianness::BIG); }                      \
  inline uint64 ReadUInt64LE(size_t offset) const { return ReadUInt64(offset, Endianness::LITTLE); }                   \
                                                                                                                       \
  inline float ReadFloatBE(size_t offset) const { return ReadFloat(offset, Endianness::BIG); }                         \
  inline float ReadFloatLE(size_t offset) const { return ReadFloat(offset, Endianness::LITTLE); }                      \
  inline double ReadDoubleBE(size_t offset) const { return ReadDouble(offset, Endianness::BIG); }                      \
  inline double ReadDoubleLE(size_t offset) const { return ReadDouble(offset, Endianness::LITTLE); }

#define __BYTES_WRITE_FUNCTIONS                                                                                        \
  inline void WriteInt8(size_t offset, int8 value) {                                                                   \
    Write<int8>(offset, value);                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteInt16(size_t offset, int16 value, Endianness endianness = Endianness::NATIVE) {                     \
    Write<int16>(offset, ConventEndiannessFromNative(value, endianness));                                              \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteInt32(size_t offset, int32 value, Endianness endianness = Endianness::NATIVE) {                     \
    Write<int32>(offset, ConventEndiannessFromNative(value, endianness));                                              \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteInt64(size_t offset, int64 value, Endianness endianness = Endianness::NATIVE) {                     \
    Write<int64>(offset, ConventEndiannessFromNative(value, endianness));                                              \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteUInt8(size_t offset, uint8 value) {                                                                 \
    Write<uint8>(offset, value);                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteUInt16(size_t offset, uint16 value, Endianness endianness = Endianness::NATIVE) {                   \
    Write<uint16>(offset, ConventEndiannessFromNative(value, endianness));                                             \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteUInt32(size_t offset, uint32 value, Endianness endianness = Endianness::NATIVE) {                   \
    Write<uint32>(offset, ConventEndiannessFromNative(value, endianness));                                             \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteUInt64(size_t offset, uint64 value, Endianness endianness = Endianness::NATIVE) {                   \
    Write<uint64>(offset, ConventEndiannessFromNative(value, endianness));                                             \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteFloat(size_t offset, float value, Endianness endianness = Endianness::NATIVE) {                     \
    return WriteUInt32(offset, ConventEndiannessFromNative(std::bit_cast<uint32>(value), endianness));                 \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteDouble(size_t offset, double value, Endianness endianness = Endianness::NATIVE) {                   \
    WriteUInt64(offset, ConventEndiannessFromNative(std::bit_cast<uint64>(value), endianness));                        \
  }                                                                                                                    \
                                                                                                                       \
  inline void WriteInt16BE(size_t offset, int16 value) { return WriteInt16(offset, value, Endianness::BIG); }          \
  inline void WriteInt16LE(size_t offset, int16 value) { return WriteInt16(offset, value, Endianness::LITTLE); }       \
  inline void WriteInt32BE(size_t offset, int32 value) { return WriteInt32(offset, value, Endianness::BIG); }          \
  inline void WriteInt32LE(size_t offset, int32 value) { return WriteInt32(offset, value, Endianness::LITTLE); }       \
  inline void WriteInt64BE(size_t offset, int64 value) { return WriteInt64(offset, value, Endianness::BIG); }          \
  inline void WriteInt64LE(size_t offset, int64 value) { return WriteInt64(offset, value, Endianness::LITTLE); }       \
                                                                                                                       \
  inline void WriteUInt16BE(size_t offset, uint16 value) { return WriteUInt16(offset, value, Endianness::BIG); }       \
  inline void WriteUInt16LE(size_t offset, uint16 value) { return WriteUInt16(offset, value, Endianness::LITTLE); }    \
  inline void WriteUInt32BE(size_t offset, uint32 value) { return WriteUInt32(offset, value, Endianness::BIG); }       \
  inline void WriteUInt32LE(size_t offset, uint32 value) { return WriteUInt32(offset, value, Endianness::LITTLE); }    \
  inline void WriteUInt64BE(size_t offset, uint64 value) { return WriteUInt64(offset, value, Endianness::BIG); }       \
  inline void WriteUInt64LE(size_t offset, uint64 value) { return WriteUInt64(offset, value, Endianness::LITTLE); }    \
                                                                                                                       \
  inline void WriteFloatBE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::BIG); }          \
  inline void WriteFloatLE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::LITTLE); }       \
  inline void WriteDoubleBE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::BIG); }       \
  inline void WriteDoubleLE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::LITTLE); }

/**
 * Not owned immutable memory range
 */
class Bytes final {
public:
  consteval Bytes() noexcept = default;
  constexpr Bytes(void const* data, size_t sizeInBytes) noexcept
    : m_data(data), m_size(sizeInBytes) {}

public:
  inline void const* GetData() const noexcept {
    return m_data;
  }

  inline size_t GetSize() const noexcept {
    return m_size;
  }

  constexpr Bytes GetView(size_t begin, size_t count) const noexcept {
    if (count == 0 || begin > m_size)
      return Bytes {};
    if (count > m_size - begin)
      count = m_size - begin;
    return Bytes { static_cast<uint8 const*>(GetData()) + begin, count };
  }

  template <typename T>
  inline T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size)
      throw std::out_of_range("offset passed to Bytes::Read() is out of range");

    auto const source = static_cast<uint8 const*>(m_data) + offset;

    if constexpr (sizeof(T) <= 8) {
      return *reinterpret_cast<T const*>(source);
    }
    else {
      T result;
      MemoryUtils::CopyBytes(&result, source, sizeof(T));
      return result;
    }
  }

  __BYTES_READ_FUNCTIONS

private:
  void const* m_data = nullptr;
  size_t m_size = 0;
};

/**
 * Not owned mutable memory range
 */
class MutableBytes final {
public:
  consteval MutableBytes() noexcept = default;
  constexpr MutableBytes(void* data, size_t sizeInBytes) noexcept
    : m_data(data), m_size(sizeInBytes) {}

public:
  inline void* GetData() const noexcept {
    return m_data;
  }

  inline size_t GetSize() const noexcept {
    return m_size;
  }

  constexpr Bytes GetView() const noexcept {
    return Bytes { static_cast<uint8*>(GetData()), GetSize() };
  }

  constexpr Bytes GetView(size_t begin, size_t count) const noexcept {
    if (count == 0 || begin > m_size)
      return Bytes {};
    if (count > m_size - begin)
      count = m_size - begin;
    return Bytes { static_cast<uint8*>(GetData()) + begin, count };
  }

  constexpr explicit(false) operator Bytes() const noexcept {
    return GetView();
  }

  constexpr MutableBytes GetMutableView(size_t begin, size_t count) noexcept {
    if (count == 0 || begin > m_size)
      return MutableBytes {};
    if (count > m_size - begin)
      count = m_size - begin;
    return MutableBytes { static_cast<uint8*>(GetData()) + begin, count };
  }

  template <typename T>
  inline T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size)
      throw std::out_of_range("offset passed to MutableBytes::Read() is out of range");

    auto const source = static_cast<uint8 const*>(m_data) + offset;

    if constexpr (sizeof(T) <= 8) {
      return *reinterpret_cast<T const*>(source);
    }
    else {
      T result;
      MemoryUtils::CopyBytes(&result, source, sizeof(T));
      return result;
    }
  }

  inline void Fill(uint8 value, size_t begin = 0, size_t count = -1) const {
    if (count > m_size - begin)
      count = m_size - begin;

    MemoryUtils::FillBytes(static_cast<uint8*>(m_data) + begin, value, count);
  }

  template <typename T>
  inline void Write(size_t offset, T const& value) {
    if (offset + sizeof(T) > m_size)
      throw std::out_of_range("offset passed to MutableBytes::Write() is out of range");

    auto const dest = static_cast<uint8*>(m_data) + offset;

    if constexpr (sizeof(T) <= 8)
      *reinterpret_cast<T*>(dest) = value;
    else
      MemoryUtils::CopyBytes(dest, &value, sizeof(T));
  }

  __BYTES_READ_FUNCTIONS
  __BYTES_WRITE_FUNCTIONS

private:
  void* m_data = nullptr;
  size_t m_size = 0;
};

/**
 * Owned resizeable memory range
 */
class Buffer final {
public:
  // Create a buffer and allocate "size" bytes and fill memory with zeroes
  static Buffer Allocate(size_t size) {
    Buffer result {};
    result.Resize(size);
    result.Fill(0);
    return result;
  }

  // Create a buffer and allocate "size" bytes and DON'T fill memory with zeroes
  static Buffer AllocateGarbaged(size_t size) {
    Buffer result {};
    result.Resize(size);
    return result;
  }

public:
  consteval Buffer() noexcept = default;

  constexpr ~Buffer() noexcept {
    _Release();
  }

  constexpr Buffer(Buffer const& other)
    : m_buffer(other.m_size > 0 ? new uint8[other.m_size] : nullptr),
      m_size(other.m_size)
  {
    if (m_size > 0)
      MemoryUtils::Copy<uint8>(m_buffer, other.m_buffer, m_size);
  }

  constexpr Buffer(Buffer&& other) noexcept
    : m_buffer(other.m_buffer),
      m_size(other.m_size)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
  }

  constexpr Buffer& operator=(Buffer const& other) {
    if (m_size == other.m_size) {
      MemoryUtils::Copy<uint8>(m_buffer, other.m_buffer, m_size);
    }
    else {
      _Release();
      m_size = other.m_size;
      if (m_size > 0) {
        m_buffer = new uint8[m_size];
        MemoryUtils::Copy<uint8>(m_buffer, other.m_buffer, m_size);
      }
      else {
        m_buffer = nullptr;
      }
    }

    return *this;
  }

  constexpr Buffer& operator=(Buffer&& other) noexcept {
    _Release();
    m_buffer = other.m_buffer;
    m_size = other.m_size;
    other.m_buffer = nullptr;
    other.m_size = 0;
    return *this;
  }

public:
  constexpr void* GetBuffer() noexcept {
    return m_buffer;
  }

  constexpr void const* GetBuffer() const noexcept {
    return m_buffer;
  }

  constexpr size_t GetSize() const noexcept {
    return m_size;
  }

  constexpr Bytes GetView() const noexcept {
    return Bytes { GetBuffer(), GetSize() };
  }

  constexpr Bytes GetView(size_t begin, size_t count) const noexcept {
    if (count == 0 || begin > m_size)
      return Bytes {};
    if (count > m_size - begin)
      count = m_size - begin;
    return Bytes { m_buffer + begin, count };
  }

  constexpr explicit(false) operator Bytes() const noexcept {
    return GetView();
  }

  constexpr MutableBytes GetMutableView() noexcept {
    return MutableBytes { GetBuffer(), GetSize() };
  }

  constexpr MutableBytes GetMutableView(size_t begin, size_t count) noexcept {
    if (count == 0 || begin > m_size)
      return MutableBytes {};
    if (count > m_size - begin)
      count = m_size - begin;
    return MutableBytes { m_buffer + begin, count };
  }

  template <typename T>
  inline T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size)
      throw std::out_of_range("offset passed to Buffer::Read() is out of range");

    auto const source = m_buffer + offset;

    if constexpr (sizeof(T) <= 8) {
      return *reinterpret_cast<T const*>(source);
    }
    else {
      T result;
      MemoryUtils::CopyBytes(&result, source, sizeof(T));
      return result;
    }
  }

  inline void Fill(uint8 value, size_t begin = 0, size_t count = -1) const {
    if (count > m_size - begin)
      count = m_size - begin;

    MemoryUtils::FillBytes(m_buffer + begin, value, count);
  }

  template <typename T>
  inline void Write(size_t offset, T const& value) {
    if (offset + sizeof(T) > m_size)
      throw std::out_of_range("offset passed to Buffer::Write() is out of range");

    auto const dest = m_buffer + offset;

    if constexpr (sizeof(T) <= 8)
      *reinterpret_cast<T*>(dest) = value;
    else
      MemoryUtils::CopyBytes(dest, &value, sizeof(T));
  }

public:
  constexpr void Resize(size_t newSize) {
    auto newBuffer = new uint8[newSize];

    if (m_buffer) {
      size_t bytesToCopy = newSize > m_size ? m_size : newSize;
      MemoryUtils::Copy<uint8>(newBuffer, m_buffer, bytesToCopy);
      delete[] m_buffer;
    }

    m_buffer = newBuffer;
    m_size = newSize;
  }

  // Reallocate buffer and drop previous data
  constexpr void Reallocate(size_t newSize) {
    if (m_buffer)
      delete[] m_buffer;

    m_buffer = new uint8[newSize];
    m_size = newSize;
  }

  __BYTES_READ_FUNCTIONS
  __BYTES_WRITE_FUNCTIONS

private:
  /// Destroy all resources held by the container
  constexpr void _Release() noexcept {
    if (m_buffer)
      delete[] m_buffer;
  }

private:
  uint8* m_buffer = nullptr;
  size_t m_size = 0;
};
