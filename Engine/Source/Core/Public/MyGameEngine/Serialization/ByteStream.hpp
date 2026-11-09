#pragma once

#include <MyGameEngine/Bytes.hpp>

/**
 * A wrapper for Buffer that allows sequential reading/writing with automatic buffer growth. Note that read and write
 * pointers are **mutable** even when this class passed by a const ref.
 */
class ByteStream final {
public:
  inline void const* GetRawBuffer() const noexcept {
    return m_buffer.GetBuffer();
  }

  inline void* GetRawBuffer() noexcept {
    return m_buffer.GetBuffer();
  }

  inline size_t GetAllocatedSpaceSize() const noexcept {
    return m_buffer.GetSize();
  }

  inline size_t GetReadPointer() const noexcept {
    return m_readPointer;
  }

  inline size_t GetWritePointer() const noexcept {
    return m_readPointer;
  }

  inline size_t GetDataSize() const noexcept {
    return m_actualDataSize;
  }

  inline void SetReadPointer(size_t readPointer) const noexcept { // const is intentional
    m_readPointer = readPointer;
  }

  inline void MoveReadPointer(ssize_t offset) const noexcept { // const is intentional
    m_readPointer += offset;
  }

  inline void SetWritePointer(size_t writePointer) noexcept {
    m_writePointer = writePointer;
  }

  inline void MoveWritePointer(ssize_t offset) noexcept {
    m_writePointer += offset;
  }

  /**
   * Sets end of the data at the write pointer. Data will be either truncated or extended. If data was extended, gaps
   * will be filled with zeroes.
   */
  inline void SetEndOfData() {
    SetEndOfData(m_writePointer);
  }

  /**
   * Sets end of the data at the given offset. Data will be either truncated or extended. If data was extended, gaps
   * will be filled with zeroes. Does not affect write pointer.
   */
  inline void SetEndOfData(size_t offset) {
    _WantAllocatedSpace(offset);

    if (offset > m_actualDataSize)
      MemoryUtils::FillBytes(static_cast<uint8*>(m_buffer.GetBuffer()) + m_actualDataSize, 0, offset - m_actualDataSize);

    m_actualDataSize = offset;
  }

public:
  inline void ReadBytes(void* dest, size_t count) const {
    if (m_readPointer + count > m_actualDataSize)
      throw std::out_of_range("trying to read outside of ByteStream range");

    // SAFE Because we guarantee that `m_actualDataSize <= m_buffer.GetSize()`

    MemoryUtils::CopyBytes(dest, static_cast<uint8 const*>(m_buffer.GetBuffer()) + m_readPointer, count);
    m_readPointer += count;
  }

  template <typename T>
    requires std::is_trivially_constructible_v<T> && std::is_trivially_copyable_v<T>
  inline T Read() const {
    T value;
    ReadBytes(&value, sizeof(T));
    return value;
  }

  inline int8 ReadInt8() const {
    return Read<int8>();
  }

  inline int16 ReadInt16(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int16>(), endianness);
  }

  inline int32 ReadInt32(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int32>(), endianness);
  }

  inline int64 ReadInt64(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<int64>(), endianness);
  }

  inline uint8 ReadUInt8() const {
    return Read<uint8>();
  }

  inline uint16 ReadUInt16(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint16>(), endianness);
  }

  inline uint32 ReadUInt32(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint32>(), endianness);
  }

  inline uint64 ReadUInt64(Endianness endianness = Endianness::NATIVE) const {
    return ConventEndiannessToNative(Read<uint64>(), endianness);
  }

  inline float ReadFloat(Endianness endianness = Endianness::NATIVE) const {
    return std::bit_cast<float>(ReadUInt32(endianness));
  }

  inline double ReadDouble(Endianness endianness = Endianness::NATIVE) const {
    return std::bit_cast<double>(ReadUInt64(endianness));
  }

  inline int16 ReadInt16BE() const { return ReadInt16(Endianness::BIG); }
  inline int16 ReadInt16LE() const { return ReadInt16(Endianness::LITTLE); }
  inline int32 ReadInt32BE() const { return ReadInt32(Endianness::BIG); }
  inline int32 ReadInt32LE() const { return ReadInt32(Endianness::LITTLE); }
  inline int64 ReadInt64BE() const { return ReadInt64(Endianness::BIG); }
  inline int64 ReadInt64LE() const { return ReadInt64(Endianness::LITTLE); }

  inline uint16 ReadUInt16BE() const { return ReadUInt16(Endianness::BIG); }
  inline uint16 ReadUInt16LE() const { return ReadUInt16(Endianness::LITTLE); }
  inline uint32 ReadUInt32BE() const { return ReadUInt32(Endianness::BIG); }
  inline uint32 ReadUInt32LE() const { return ReadUInt32(Endianness::LITTLE); }
  inline uint64 ReadUInt64BE() const { return ReadUInt64(Endianness::BIG); }
  inline uint64 ReadUInt64LE() const { return ReadUInt64(Endianness::LITTLE); }

  inline float ReadFloatBE() const { return ReadFloat(Endianness::BIG); }
  inline float ReadFloatLE() const { return ReadFloat(Endianness::LITTLE); }
  inline double ReadDoubleBE() const { return ReadDouble(Endianness::BIG); }
  inline double ReadDoubleLE() const { return ReadDouble(Endianness::LITTLE); }

public:
  inline void WriteBytes(void const* source, size_t count) {
    _WantWriteAtWritePointer(count);

    if (m_writePointer > m_actualDataSize) {
      // fill the gap with zeroes
      MemoryUtils::FillBytes(static_cast<uint8*>(m_buffer.GetBuffer()) + m_actualDataSize, 0,
        m_writePointer - m_actualDataSize);
    }

    MemoryUtils::CopyBytes(static_cast<uint8*>(m_buffer.GetBuffer()) + m_writePointer, source, count);
    m_writePointer += count;

    if (m_actualDataSize < m_writePointer)
      m_actualDataSize = m_writePointer;
  }

  template <typename T>
    requires std::is_trivially_constructible_v<T> && std::is_trivially_copyable_v<T>
  inline void Write(std::type_identity_t<T> value) {
    WriteBytes(&value, sizeof(T));
  }

  inline void WriteInt8(int8 value) {
    Write<int8>(value);
  }

  inline void WriteInt16(int16 value, Endianness endianness = Endianness::NATIVE) {
    Write<int16>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteInt32(int32 value, Endianness endianness = Endianness::NATIVE) {
    Write<int32>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteInt64(int64 value, Endianness endianness = Endianness::NATIVE) {
    Write<int64>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt8(uint8 value) {
    Write<uint8>(value);
  }

  inline void WriteUInt16(uint16 value, Endianness endianness = Endianness::NATIVE) {
    Write<uint16>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt32(uint32 value, Endianness endianness = Endianness::NATIVE) {
    Write<uint32>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteUInt64(uint64 value, Endianness endianness = Endianness::NATIVE) {
    Write<uint64>(ConventEndiannessFromNative(value, endianness));
  }

  inline void WriteFloat(float value, Endianness endianness = Endianness::NATIVE) {
    Write<uint32>(ConventEndiannessFromNative(std::bit_cast<uint32>(value), endianness));
  }

  inline void WriteDouble(double value, Endianness endianness = Endianness::NATIVE) {
    Write<uint64>(ConventEndiannessFromNative(std::bit_cast<uint64>(value), endianness));
  }

  inline void WriteInt16BE(int16 value) { WriteInt16(value, Endianness::BIG); }
  inline void WriteInt16LE(int16 value) { WriteInt16(value, Endianness::LITTLE); }
  inline void WriteInt32BE(int32 value) { WriteInt32(value, Endianness::BIG); }
  inline void WriteInt32LE(int32 value) { WriteInt32(value, Endianness::LITTLE); }
  inline void WriteInt64BE(int64 value) { WriteInt64(value, Endianness::BIG); }
  inline void WriteInt64LE(int64 value) { WriteInt64(value, Endianness::LITTLE); }

  inline void WriteUInt16BE(uint16 value) { WriteUInt16(value, Endianness::BIG); }
  inline void WriteUInt16LE(uint16 value) { WriteUInt16(value, Endianness::LITTLE); }
  inline void WriteUInt32BE(uint32 value) { WriteUInt32(value, Endianness::BIG); }
  inline void WriteUInt32LE(uint32 value) { WriteUInt32(value, Endianness::LITTLE); }
  inline void WriteUInt64BE(uint64 value) { WriteUInt64(value, Endianness::BIG); }
  inline void WriteUInt64LE(uint64 value) { WriteUInt64(value, Endianness::LITTLE); }

  inline void WriteFloatBE(float value) { WriteFloat(value, Endianness::BIG); }
  inline void WriteFloatLE(float value) { WriteFloat(value, Endianness::LITTLE); }
  inline void WriteDoubleBE(double value) { WriteDouble(value, Endianness::BIG); }
  inline void WriteDoubleLE(double value) { WriteDouble(value, Endianness::LITTLE); }

private:
  inline void _WantWriteAtWritePointer(size_t bytesCount) {
    _WantAllocatedSpace(m_writePointer + bytesCount);
  }

  inline void _WantAllocatedSpace(size_t requiredAllocatedSpace) {
    if (m_buffer.GetSize() < requiredAllocatedSpace) {
      size_t newCapacity = m_buffer.GetSize() * 2;
      if (newCapacity < requiredAllocatedSpace)
        newCapacity = requiredAllocatedSpace;

      m_buffer.Resize(newCapacity);
    }
  }

private:
  Buffer m_buffer;
  size_t m_actualDataSize = 0;
  mutable size_t m_readPointer = 0;
  size_t m_writePointer = 0;
};
