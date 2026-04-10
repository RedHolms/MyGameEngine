#pragma once

#include "Memory/Bits.hpp"
#include "Memory/Utils.hpp"
#include "Types/Casts.hpp"

/**
 * Resizeable memory range
 */
class Buffer {
public:
  using ItemType = uint8_t;

public:
  consteval Buffer() = default;

  constexpr ~Buffer() noexcept {
    _Reset();
  }

  // Create a buffer and allocate "preallocate" bytes and fill memory with zeroes
  static Buffer Allocate(size_t preallocate) noexcept {
    Buffer result {};
    result.Resize(preallocate);
    result.Fill(0);
    return result;
  }

  constexpr Buffer(Buffer const& other) noexcept
    : m_buffer(other.m_size > 0 ? new uint8_t[other.m_size] : nullptr),
      m_size(other.m_size)
  {
    if (m_size > 0)
      Memory::CopyItems<uint8_t>(m_buffer, other.m_buffer, m_size);
  }

  constexpr Buffer(Buffer&& other) noexcept
    : m_buffer(other.m_buffer),
      m_size(other.m_size)
  {
    other.m_buffer = nullptr;
    other.m_size = 0;
  }

  constexpr Buffer& operator=(Buffer const& other) noexcept {
    if (m_size == other.m_size) {
      Memory::CopyItems<uint8_t>(m_buffer, other.m_buffer, m_size);
    }
    else {
      _Reset();
      m_size = other.m_size;
      if (m_size > 0) {
        m_buffer = new uint8_t[m_size];
        Memory::CopyItems<uint8_t>(m_buffer, other.m_buffer, m_size);
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

  template <TriviallyCopyableType T>
  constexpr T Read(index_t offset) const noexcept {
    size_t realOffset = _TranslateOffset(offset);
    if (realOffset + sizeof(T) > m_size) {
      // TODO throw an error: out of range
      return {};
    }

    if constexpr (sizeof(T) <= 8)
      return *reinterpret_cast<T*>(m_buffer + realOffset);
    else {
      T result;
      Memory::CopyItems<T>(&result, reinterpret_cast<T const*>(m_buffer + realOffset), 1);
      return result;
    }
  }

  constexpr int8_t ReadInt8(index_t offset) const noexcept {
    return Read<int8_t>(offset);
  }

  constexpr int16_t ReadInt16(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<int16_t>(offset), endianness);
  }

  constexpr int32_t ReadInt32(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<int32_t>(offset), endianness);
  }

  constexpr int64_t ReadInt64(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<int64_t>(offset), endianness);
  }

  constexpr uint8_t ReadUInt8(index_t offset) const noexcept {
    return Read<uint8_t>(offset);
  }

  constexpr uint16_t ReadUInt16(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<uint16_t>(offset), endianness);
  }

  constexpr uint32_t ReadUInt32(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<uint32_t>(offset), endianness);
  }

  constexpr uint64_t ReadUInt64(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return ConventEndiannessToNative(Read<uint64_t>(offset), endianness);
  }

  constexpr float ReadFloat(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return bit_cast<float>(ReadUInt32(offset, endianness));
  }

  constexpr double ReadDouble(index_t offset, Endianness endianness = Endianness::NATIVE) const noexcept {
    return bit_cast<double>(ReadUInt64(offset, endianness));
  }

  constexpr int16_t ReadInt16BE(index_t offset) const noexcept { return ReadInt16(offset, Endianness::BIG); }
  constexpr int16_t ReadInt16LE(index_t offset) const noexcept { return ReadInt16(offset, Endianness::LITTLE); }
  constexpr int32_t ReadInt32BE(index_t offset) const noexcept { return ReadInt32(offset, Endianness::BIG); }
  constexpr int32_t ReadInt32LE(index_t offset) const noexcept { return ReadInt32(offset, Endianness::LITTLE); }
  constexpr int64_t ReadInt64BE(index_t offset) const noexcept { return ReadInt64(offset, Endianness::BIG); }
  constexpr int64_t ReadInt64LE(index_t offset) const noexcept { return ReadInt64(offset, Endianness::LITTLE); }

  constexpr uint16_t ReadUInt16BE(index_t offset) const noexcept { return ReadUInt16(offset, Endianness::BIG); }
  constexpr uint16_t ReadUInt16LE(index_t offset) const noexcept { return ReadUInt16(offset, Endianness::LITTLE); }
  constexpr uint32_t ReadUInt32BE(index_t offset) const noexcept { return ReadUInt32(offset, Endianness::BIG); }
  constexpr uint32_t ReadUInt32LE(index_t offset) const noexcept { return ReadUInt32(offset, Endianness::LITTLE); }
  constexpr uint64_t ReadUInt64BE(index_t offset) const noexcept { return ReadUInt64(offset, Endianness::BIG); }
  constexpr uint64_t ReadUInt64LE(index_t offset) const noexcept { return ReadUInt64(offset, Endianness::LITTLE); }

  constexpr float ReadFloatBE(index_t offset) const noexcept { return ReadFloat(offset, Endianness::BIG); }
  constexpr float ReadFloatLE(index_t offset) const noexcept { return ReadFloat(offset, Endianness::LITTLE); }
  constexpr double ReadDoubleBE(index_t offset) const noexcept { return ReadDouble(offset, Endianness::BIG); }
  constexpr double ReadDoubleLE(index_t offset) const noexcept { return ReadDouble(offset, Endianness::LITTLE); }

public:
  constexpr void Resize(size_t newSize) noexcept {
    auto newBuffer = new uint8_t[newSize];

    if (m_buffer) {
      size_t bytesToCopy = newSize > m_size ? m_size : newSize;
      Memory::CopyItems<uint8_t>(newBuffer, m_buffer, bytesToCopy);
      delete[] m_buffer;
    }

    m_buffer = newBuffer;
    m_size = newSize;
  }

  constexpr void Fill(uint8_t value, index_t begin = 0, size_t count = -1) noexcept {
    size_t realBegin = _TranslateOffset(begin);
    if (count > m_size - realBegin)
      count = m_size;

    if (IsConstantEvaluated()) {
      size_t end = realBegin + count;
      for (size_t i = realBegin; i < end; ++i)
        m_buffer[i] = value;
    }
    else {
      Memory::FillBytes(m_buffer + realBegin, value, count);
    }
  }

  template <TriviallyCopyableType T>
  constexpr void Write(index_t offset, T const& value) noexcept {
    size_t realOffset = _TranslateOffset(offset);
    if (realOffset + sizeof(T) > m_size) {
      // TODO throw an error: out of range
      return;
    }

    if constexpr (sizeof(T) <= 8)
      *reinterpret_cast<T*>(m_buffer + realOffset) = value;
    else
      Memory::CopyItems<T>(reinterpret_cast<T*>(m_buffer + realOffset), &value, 1);
  }

  constexpr void WriteInt8(index_t offset, int8_t value) noexcept {
    return Write<int8_t>(offset, value);
  }

  constexpr void WriteInt16(index_t offset, int16_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<int16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteInt32(index_t offset, int32_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<int32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteInt64(index_t offset, int64_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<int64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt8(index_t offset, uint8_t value) noexcept {
    return Write<uint8_t>(offset, value);
  }

  constexpr void WriteUInt16(index_t offset, uint16_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<uint16_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt32(index_t offset, uint32_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<uint32_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteUInt64(index_t offset, uint64_t value, Endianness endianness = Endianness::NATIVE) noexcept {
    return Write<uint64_t>(offset, ConventEndiannessFromNative(value, endianness));
  }

  constexpr void WriteFloat(index_t offset, float value, Endianness endianness = Endianness::NATIVE) noexcept {
    return WriteUInt32(offset, ConventEndiannessFromNative(bit_cast<uint32_t>(value), endianness));
  }

  constexpr void WriteDouble(index_t offset, double value, Endianness endianness = Endianness::NATIVE) noexcept {
    return WriteUInt64(offset, ConventEndiannessFromNative(bit_cast<uint64_t>(value), endianness));
  }

  constexpr void WriteInt16BE(index_t offset, int16_t value) noexcept { return WriteInt16(offset, value, Endianness::BIG); }
  constexpr void WriteInt16LE(index_t offset, int16_t value) noexcept { return WriteInt16(offset, value, Endianness::LITTLE); }
  constexpr void WriteInt32BE(index_t offset, int32_t value) noexcept { return WriteInt32(offset, value, Endianness::BIG); }
  constexpr void WriteInt32LE(index_t offset, int32_t value) noexcept { return WriteInt32(offset, value, Endianness::LITTLE); }
  constexpr void WriteInt64BE(index_t offset, int64_t value) noexcept { return WriteInt64(offset, value, Endianness::BIG); }
  constexpr void WriteInt64LE(index_t offset, int64_t value) noexcept { return WriteInt64(offset, value, Endianness::LITTLE); }

  constexpr void WriteUInt16BE(index_t offset, uint16_t value) noexcept { return WriteUInt16(offset, value, Endianness::BIG); }
  constexpr void WriteUInt16LE(index_t offset, uint16_t value) noexcept { return WriteUInt16(offset, value, Endianness::LITTLE); }
  constexpr void WriteUInt32BE(index_t offset, uint32_t value) noexcept { return WriteUInt32(offset, value, Endianness::BIG); }
  constexpr void WriteUInt32LE(index_t offset, uint32_t value) noexcept { return WriteUInt32(offset, value, Endianness::LITTLE); }
  constexpr void WriteUInt64BE(index_t offset, uint64_t value) noexcept { return WriteUInt64(offset, value, Endianness::BIG); }
  constexpr void WriteUInt64LE(index_t offset, uint64_t value) noexcept { return WriteUInt64(offset, value, Endianness::LITTLE); }

  constexpr void WriteFloatBE(index_t offset, float value) noexcept { return WriteFloat(offset, value, Endianness::BIG); }
  constexpr void WriteFloatLE(index_t offset, float value) noexcept { return WriteFloat(offset, value, Endianness::LITTLE); }
  constexpr void WriteDoubleBE(index_t offset, double value) noexcept { return WriteDouble(offset, value, Endianness::BIG); }
  constexpr void WriteDoubleLE(index_t offset, double value) noexcept { return WriteDouble(offset, value, Endianness::LITTLE); }

private:
  constexpr size_t _TranslateOffset(index_t index) const noexcept {
    if (index < 0)
      return m_size + index;
    return index;
  }

  constexpr void _Reset() noexcept {
    if (m_buffer)
      delete[] m_buffer;
  }

private:
  uint8_t* m_buffer = nullptr;
  size_t m_size = 0;
};
