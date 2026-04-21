#pragma once

#include "Endianness.hpp"
#include "Memory/Utils.hpp"
#include "Types/Casts.hpp"

enum class ByteAccessType {
  READ,
  WRITE
};

// For Bytes, MutableBytes and Buffer
class ByteAccessOutOfRange final : public RuntimeException {
public:
  inline ByteAccessOutOfRange(
    ByteAccessType accessType,
    void const* buffer, size_t bufferSize,
    size_t accessOffset,
    size_t accessSize
  ) : RuntimeException(U"Bytes container access out of range"s),
      m_accessType(accessType),
      m_buffer(buffer),
      m_bufferSize(bufferSize),
      m_accessOffset(accessOffset),
      m_accessSize(accessSize)
  {}

public:
  inline TypeInfo const& GetExceptionType() const noexcept override {
    return typeid(ByteAccessOutOfRange);
  }

  inline ByteAccessType GetAccessType() const {
    return m_accessType;
  }

  inline void const* GetBuffer() const {
    return m_buffer;
  }

  inline size_t GetBufferSize() const {
    return m_bufferSize;
  }

  inline size_t GetAccessOffset() const {
    return m_accessOffset;
  }

  inline size_t GetAccessSize() const {
    return m_accessSize;
  }

private:
  ByteAccessType m_accessType;

  // Buffer we tried to access:
  void const* const m_buffer;
  size_t const m_bufferSize;

  size_t m_accessOffset;
  size_t m_accessSize;
};

/**
 * Not owned immutable memory range
 */
class Bytes {
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

  template <TT::BitwiseRelocatableType T>
  inline T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size) {
      throw ByteAccessOutOfRange(
        ByteAccessType::READ,
        m_data, m_size,
        offset, sizeof(T)
      );
    }

    if constexpr (sizeof(T) <= 8)
      return *reinterpret_cast<T const*>(static_cast<uint8_t const*>(m_data) + offset);
    else {
      T result;
      Memory::Copy<T>(&result, static_cast<T const*>(static_cast<uint8_t const*>(m_data) + offset), 1);
      return result;
    }
  }

  inline int8_t ReadInt8(size_t offset) const {
    return Read<int8_t>(offset);
  }

  inline int16_t ReadInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int16_t>(offset), endianness);
  }

  inline int32_t ReadInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int32_t>(offset), endianness);
  }

  inline int64_t ReadInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int64_t>(offset), endianness);
  }

  inline uint8_t ReadUInt8(size_t offset) const {
    return Read<uint8_t>(offset);
  }

  inline uint16_t ReadUInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint16_t>(offset), endianness);
  }

  inline uint32_t ReadUInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint32_t>(offset), endianness);
  }

  inline uint64_t ReadUInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint64_t>(offset), endianness);
  }

  inline float ReadFloat(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<float>(ReadUInt32(offset, endianness));
  }

  inline double ReadDouble(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<double>(ReadUInt64(offset, endianness));
  }

  inline int16_t ReadInt16BE(size_t offset) const { return ReadInt16(offset, Endianness::BIG); }
  inline int16_t ReadInt16LE(size_t offset) const { return ReadInt16(offset, Endianness::LITTLE); }
  inline int32_t ReadInt32BE(size_t offset) const { return ReadInt32(offset, Endianness::BIG); }
  inline int32_t ReadInt32LE(size_t offset) const { return ReadInt32(offset, Endianness::LITTLE); }
  inline int64_t ReadInt64BE(size_t offset) const { return ReadInt64(offset, Endianness::BIG); }
  inline int64_t ReadInt64LE(size_t offset) const { return ReadInt64(offset, Endianness::LITTLE); }

  inline uint16_t ReadUInt16BE(size_t offset) const { return ReadUInt16(offset, Endianness::BIG); }
  inline uint16_t ReadUInt16LE(size_t offset) const { return ReadUInt16(offset, Endianness::LITTLE); }
  inline uint32_t ReadUInt32BE(size_t offset) const { return ReadUInt32(offset, Endianness::BIG); }
  inline uint32_t ReadUInt32LE(size_t offset) const { return ReadUInt32(offset, Endianness::LITTLE); }
  inline uint64_t ReadUInt64BE(size_t offset) const { return ReadUInt64(offset, Endianness::BIG); }
  inline uint64_t ReadUInt64LE(size_t offset) const { return ReadUInt64(offset, Endianness::LITTLE); }

  inline float ReadFloatBE(size_t offset) const { return ReadFloat(offset, Endianness::BIG); }
  inline float ReadFloatLE(size_t offset) const { return ReadFloat(offset, Endianness::LITTLE); }
  inline double ReadDoubleBE(size_t offset) const { return ReadDouble(offset, Endianness::BIG); }
  inline double ReadDoubleLE(size_t offset) const { return ReadDouble(offset, Endianness::LITTLE); }

private:
  void const* m_data = nullptr;
  size_t m_size = 0;
};

/**
 * Not owned mutable memory range
 */
class MutableBytes {
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

  template <TT::BitwiseRelocatableType T>
  inline T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size) {
      throw ByteAccessOutOfRange(
        ByteAccessType::READ,
        m_data, m_size,
        offset, sizeof(T)
      );
    }

    if constexpr (sizeof(T) <= 8)
      return *reinterpret_cast<T const*>(static_cast<uint8_t const*>(m_data) + offset);
    else {
      T result;
      Memory::Copy<T>(&result, static_cast<T const*>(static_cast<uint8_t const*>(m_data) + offset), 1);
      return result;
    }
  }

  inline int8_t ReadInt8(size_t offset) const {
    return Read<int8_t>(offset);
  }

  inline int16_t ReadInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int16_t>(offset), endianness);
  }

  inline int32_t ReadInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int32_t>(offset), endianness);
  }

  inline int64_t ReadInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int64_t>(offset), endianness);
  }

  inline uint8_t ReadUInt8(size_t offset) const {
    return Read<uint8_t>(offset);
  }

  inline uint16_t ReadUInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint16_t>(offset), endianness);
  }

  inline uint32_t ReadUInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint32_t>(offset), endianness);
  }

  inline uint64_t ReadUInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint64_t>(offset), endianness);
  }

  inline float ReadFloat(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<float>(ReadUInt32(offset, endianness));
  }

  inline double ReadDouble(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<double>(ReadUInt64(offset, endianness));
  }

  inline int16_t ReadInt16BE(size_t offset) const { return ReadInt16(offset, Endianness::BIG); }
  inline int16_t ReadInt16LE(size_t offset) const { return ReadInt16(offset, Endianness::LITTLE); }
  inline int32_t ReadInt32BE(size_t offset) const { return ReadInt32(offset, Endianness::BIG); }
  inline int32_t ReadInt32LE(size_t offset) const { return ReadInt32(offset, Endianness::LITTLE); }
  inline int64_t ReadInt64BE(size_t offset) const { return ReadInt64(offset, Endianness::BIG); }
  inline int64_t ReadInt64LE(size_t offset) const { return ReadInt64(offset, Endianness::LITTLE); }

  inline uint16_t ReadUInt16BE(size_t offset) const { return ReadUInt16(offset, Endianness::BIG); }
  inline uint16_t ReadUInt16LE(size_t offset) const { return ReadUInt16(offset, Endianness::LITTLE); }
  inline uint32_t ReadUInt32BE(size_t offset) const { return ReadUInt32(offset, Endianness::BIG); }
  inline uint32_t ReadUInt32LE(size_t offset) const { return ReadUInt32(offset, Endianness::LITTLE); }
  inline uint64_t ReadUInt64BE(size_t offset) const { return ReadUInt64(offset, Endianness::BIG); }
  inline uint64_t ReadUInt64LE(size_t offset) const { return ReadUInt64(offset, Endianness::LITTLE); }

  inline float ReadFloatBE(size_t offset) const { return ReadFloat(offset, Endianness::BIG); }
  inline float ReadFloatLE(size_t offset) const { return ReadFloat(offset, Endianness::LITTLE); }
  inline double ReadDoubleBE(size_t offset) const { return ReadDouble(offset, Endianness::BIG); }
  inline double ReadDoubleLE(size_t offset) const { return ReadDouble(offset, Endianness::LITTLE); }

  inline void Fill(uint8_t value, size_t begin = 0, size_t count = -1) {
    if (count > m_size - begin)
      count = m_size - begin;

    Memory::FillBytes(static_cast<uint8_t*>(m_data) + begin, value, count);
  }

  template <TT::BitwiseRelocatableType T>
  inline void Write(size_t offset, T const& value) {
    if (offset + sizeof(T) > m_size) {
      throw ByteAccessOutOfRange(
        ByteAccessType::WRITE,
        m_data, m_size,
        offset, sizeof(T)
      );
    }

    if constexpr (sizeof(T) <= 8)
      *reinterpret_cast<T*>(static_cast<uint8_t*>(m_data) + offset) = value;
    else
      Memory::Copy<T>(reinterpret_cast<T*>(static_cast<uint8_t*>(m_data) + offset), &value, 1);
  }

  inline void WriteInt8(size_t offset, int8_t value) {
    return Write<int8_t>(offset, value);
  }

  inline void WriteInt16(size_t offset, int16_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteInt32(size_t offset, int32_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteInt64(size_t offset, int64_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt8(size_t offset, uint8_t value) {
    return Write<uint8_t>(offset, value);
  }

  inline void WriteUInt16(size_t offset, uint16_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt32(size_t offset, uint32_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt64(size_t offset, uint64_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteFloat(size_t offset, float value, Endianness endianness = Endianness::NATIVE) {
    return WriteUInt32(offset, ConventEndiannessFromNative(bit_cast<uint32_t>(value), endianness));
  }

  inline void WriteDouble(size_t offset, double value, Endianness endianness = Endianness::NATIVE) {
    return WriteUInt64(offset, ConventEndiannessFromNative(bit_cast<uint64_t>(value), endianness));
  }

  inline void WriteInt16BE(size_t offset, int16_t value) { return WriteInt16(offset, value, Endianness::BIG); }
  inline void WriteInt16LE(size_t offset, int16_t value) { return WriteInt16(offset, value, Endianness::LITTLE); }
  inline void WriteInt32BE(size_t offset, int32_t value) { return WriteInt32(offset, value, Endianness::BIG); }
  inline void WriteInt32LE(size_t offset, int32_t value) { return WriteInt32(offset, value, Endianness::LITTLE); }
  inline void WriteInt64BE(size_t offset, int64_t value) { return WriteInt64(offset, value, Endianness::BIG); }
  inline void WriteInt64LE(size_t offset, int64_t value) { return WriteInt64(offset, value, Endianness::LITTLE); }

  inline void WriteUInt16BE(size_t offset, uint16_t value) { return WriteUInt16(offset, value, Endianness::BIG); }
  inline void WriteUInt16LE(size_t offset, uint16_t value) { return WriteUInt16(offset, value, Endianness::LITTLE); }
  inline void WriteUInt32BE(size_t offset, uint32_t value) { return WriteUInt32(offset, value, Endianness::BIG); }
  inline void WriteUInt32LE(size_t offset, uint32_t value) { return WriteUInt32(offset, value, Endianness::LITTLE); }
  inline void WriteUInt64BE(size_t offset, uint64_t value) { return WriteUInt64(offset, value, Endianness::BIG); }
  inline void WriteUInt64LE(size_t offset, uint64_t value) { return WriteUInt64(offset, value, Endianness::LITTLE); }

  inline void WriteFloatBE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::BIG); }
  inline void WriteFloatLE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::LITTLE); }
  inline void WriteDoubleBE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::BIG); }
  inline void WriteDoubleLE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::LITTLE); }

private:
  void* m_data = nullptr;
  size_t m_size = 0;
};

/**
 * Owned resizeable memory range
 */
class Buffer {
public:
  using ItemType = uint8_t;

public:
  consteval Buffer() noexcept = default;

  constexpr ~Buffer() noexcept {
    _Reset();
  }

  // Create a buffer and allocate "preallocate" bytes and fill memory with zeroes
  static Buffer Allocate(size_t preallocate) {
    Buffer result {};
    result.Resize(preallocate);
    result.Fill(0);
    return result;
  }

  // Create a buffer and allocate "preallocate" bytes and NOT fill memory with zeroes
  static Buffer AllocateGarbaged(size_t preallocate) {
    Buffer result {};
    result.Resize(preallocate);
    return result;
  }

  constexpr Buffer(Buffer const& other)
    : m_buffer(other.m_size > 0 ? new uint8_t[other.m_size] : nullptr),
      m_size(other.m_size)
  {
    if (m_size > 0)
      Memory::Copy<uint8_t>(m_buffer, other.m_buffer, m_size);
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
      Memory::Copy<uint8_t>(m_buffer, other.m_buffer, m_size);
    }
    else {
      _Reset();
      m_size = other.m_size;
      if (m_size > 0) {
        m_buffer = new uint8_t[m_size];
        Memory::Copy<uint8_t>(m_buffer, other.m_buffer, m_size);
      }
      else {
        m_buffer = nullptr;
      }
    }

    return *this;
  }

  constexpr Buffer& operator=(Buffer&& other) noexcept {
    _Reset();
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
    return Bytes { m_buffer, m_size };
  }

  template <TT::BitwiseRelocatableType T>
  constexpr T Read(size_t offset) const {
    if (offset + sizeof(T) > m_size) {
      throw ByteAccessOutOfRange(
        ByteAccessType::READ,
        m_buffer, m_size,
        offset, sizeof(T)
      );
    }

    if constexpr (sizeof(T) <= 8)
      return *reinterpret_cast<T const*>(m_buffer + offset);
    else {
      T result;
      Memory::Copy<T>(&result, reinterpret_cast<T const*>(m_buffer + offset), 1);
      return result;
    }
  }

  constexpr int8_t ReadInt8(size_t offset) const {
    return Read<int8_t>(offset);
  }

  constexpr int16_t ReadInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int16_t>(offset), endianness);
  }

  constexpr int32_t ReadInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int32_t>(offset), endianness);
  }

  constexpr int64_t ReadInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int64_t>(offset), endianness);
  }

  constexpr uint8_t ReadUInt8(size_t offset) const {
    return Read<uint8_t>(offset);
  }

  constexpr uint16_t ReadUInt16(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint16_t>(offset), endianness);
  }

  constexpr uint32_t ReadUInt32(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint32_t>(offset), endianness);
  }

  constexpr uint64_t ReadUInt64(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint64_t>(offset), endianness);
  }

  constexpr float ReadFloat(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<float>(ReadUInt32(offset, endianness));
  }

  constexpr double ReadDouble(size_t offset, Endianness endianness = Endianness::NATIVE) const {
    return bit_cast<double>(ReadUInt64(offset, endianness));
  }

  constexpr int16_t ReadInt16BE(size_t offset) const { return ReadInt16(offset, Endianness::BIG); }
  constexpr int16_t ReadInt16LE(size_t offset) const { return ReadInt16(offset, Endianness::LITTLE); }
  constexpr int32_t ReadInt32BE(size_t offset) const { return ReadInt32(offset, Endianness::BIG); }
  constexpr int32_t ReadInt32LE(size_t offset) const { return ReadInt32(offset, Endianness::LITTLE); }
  constexpr int64_t ReadInt64BE(size_t offset) const { return ReadInt64(offset, Endianness::BIG); }
  constexpr int64_t ReadInt64LE(size_t offset) const { return ReadInt64(offset, Endianness::LITTLE); }

  constexpr uint16_t ReadUInt16BE(size_t offset) const { return ReadUInt16(offset, Endianness::BIG); }
  constexpr uint16_t ReadUInt16LE(size_t offset) const { return ReadUInt16(offset, Endianness::LITTLE); }
  constexpr uint32_t ReadUInt32BE(size_t offset) const { return ReadUInt32(offset, Endianness::BIG); }
  constexpr uint32_t ReadUInt32LE(size_t offset) const { return ReadUInt32(offset, Endianness::LITTLE); }
  constexpr uint64_t ReadUInt64BE(size_t offset) const { return ReadUInt64(offset, Endianness::BIG); }
  constexpr uint64_t ReadUInt64LE(size_t offset) const { return ReadUInt64(offset, Endianness::LITTLE); }

  constexpr float ReadFloatBE(size_t offset) const { return ReadFloat(offset, Endianness::BIG); }
  constexpr float ReadFloatLE(size_t offset) const { return ReadFloat(offset, Endianness::LITTLE); }
  constexpr double ReadDoubleBE(size_t offset) const { return ReadDouble(offset, Endianness::BIG); }
  constexpr double ReadDoubleLE(size_t offset) const { return ReadDouble(offset, Endianness::LITTLE); }

public:
  constexpr void Resize(size_t newSize) {
    auto newBuffer = new uint8_t[newSize];

    if (m_buffer) {
      size_t bytesToCopy = newSize > m_size ? m_size : newSize;
      Memory::Copy<uint8_t>(newBuffer, m_buffer, bytesToCopy);
      delete[] m_buffer;
    }

    m_buffer = newBuffer;
    m_size = newSize;
  }

  // Reallocate buffer and drop previous data
  constexpr void Reallocate(size_t newSize) {
    if (m_buffer)
      delete[] m_buffer;

    m_buffer = new uint8_t[newSize];
    m_size = newSize;
  }

  constexpr void Fill(uint8_t value, size_t begin = 0, size_t count = -1) {
    if (count > m_size - begin)
      count = m_size - begin;

    if (IsConstantEvaluated()) {
      size_t end = begin + count;
      for (size_t i = begin; i < end; ++i)
        m_buffer[i] = value;
    }
    else {
      Memory::FillBytes(m_buffer + begin, value, count);
    }
  }

  template <TT::BitwiseRelocatableType T>
  constexpr void Write(size_t offset, T const& value) {
    if (offset + sizeof(T) > m_size) {
      throw ByteAccessOutOfRange(
        ByteAccessType::WRITE,
        m_buffer, m_size,
        offset, sizeof(T)
      );
    }

    if constexpr (sizeof(T) <= 8)
      *reinterpret_cast<T*>(m_buffer + offset) = value;
    else
      Memory::Copy<T>(reinterpret_cast<T*>(m_buffer + offset), &value, 1);
  }

  constexpr void WriteInt8(size_t offset, int8_t value) {
    return Write<int8_t>(offset, value);
  }

  constexpr void WriteInt16(size_t offset, int16_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteInt32(size_t offset, int32_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteInt64(size_t offset, int64_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<int64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt8(size_t offset, uint8_t value) {
    return Write<uint8_t>(offset, value);
  }

  constexpr void WriteUInt16(size_t offset, uint16_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt32(size_t offset, uint32_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt64(size_t offset, uint64_t value, Endianness endianness = Endianness::NATIVE) {
    return Write<uint64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteFloat(size_t offset, float value, Endianness endianness = Endianness::NATIVE) {
    return WriteUInt32(offset, ConventEndiannessFromNative(bit_cast<uint32_t>(value), endianness));
  }

  constexpr void WriteDouble(size_t offset, double value, Endianness endianness = Endianness::NATIVE) {
    return WriteUInt64(offset, ConventEndiannessFromNative(bit_cast<uint64_t>(value), endianness));
  }

  constexpr void WriteInt16BE(size_t offset, int16_t value) { return WriteInt16(offset, value, Endianness::BIG); }
  constexpr void WriteInt16LE(size_t offset, int16_t value) { return WriteInt16(offset, value, Endianness::LITTLE); }
  constexpr void WriteInt32BE(size_t offset, int32_t value) { return WriteInt32(offset, value, Endianness::BIG); }
  constexpr void WriteInt32LE(size_t offset, int32_t value) { return WriteInt32(offset, value, Endianness::LITTLE); }
  constexpr void WriteInt64BE(size_t offset, int64_t value) { return WriteInt64(offset, value, Endianness::BIG); }
  constexpr void WriteInt64LE(size_t offset, int64_t value) { return WriteInt64(offset, value, Endianness::LITTLE); }

  constexpr void WriteUInt16BE(size_t offset, uint16_t value) { return WriteUInt16(offset, value, Endianness::BIG); }
  constexpr void WriteUInt16LE(size_t offset, uint16_t value) { return WriteUInt16(offset, value, Endianness::LITTLE); }
  constexpr void WriteUInt32BE(size_t offset, uint32_t value) { return WriteUInt32(offset, value, Endianness::BIG); }
  constexpr void WriteUInt32LE(size_t offset, uint32_t value) { return WriteUInt32(offset, value, Endianness::LITTLE); }
  constexpr void WriteUInt64BE(size_t offset, uint64_t value) { return WriteUInt64(offset, value, Endianness::BIG); }
  constexpr void WriteUInt64LE(size_t offset, uint64_t value) { return WriteUInt64(offset, value, Endianness::LITTLE); }

  constexpr void WriteFloatBE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::BIG); }
  constexpr void WriteFloatLE(size_t offset, float value) { return WriteFloat(offset, value, Endianness::LITTLE); }
  constexpr void WriteDoubleBE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::BIG); }
  constexpr void WriteDoubleLE(size_t offset, double value) { return WriteDouble(offset, value, Endianness::LITTLE); }

private:
  constexpr void _Reset() noexcept {
    if (m_buffer)
      delete[] m_buffer;
  }

private:
  uint8_t* m_buffer = nullptr;
  size_t m_size = 0;
};
