#pragma once

#include <MyGameEngine/String.hpp>
#include <expected>
#include <future>

/**
 * Interface to an object that provides synchronous functions to read bytes from something of known size. E.g: a file or
 * a memory buffer.
 */
class ISyncRandomAccessByteSource {
public:
  virtual ~ISyncRandomAccessByteSource() = 0;

public:
  virtual size_t GetDataSize() const = 0;

  // I.e. will return false for files
  virtual bool IsPlacedInMemory() const = 0;

  virtual std::expected<void, String> ReadBytesTo(size_t offset, void* outBuffer, size_t bytesCount) const = 0;
};

/**
 * Interface to an object that provides asynchronous functions to read bytes from something of known size. E.g: a file
 * or a memory buffer.
 */
class IAsyncRandomAccessByteSource {
public:
  virtual ~IAsyncRandomAccessByteSource() = 0;

public:
  virtual std::future<size_t> GetDataSize() const = 0;

  // I.e. will return false for files
  virtual bool IsPlacedInMemory() const = 0;

  // "outBuffer" obviously must be alive until returned future is resolved
  virtual std::future<std::expected<void, String>> ReadBytesTo(size_t offset, void* outBuffer, size_t bytesCount)
    const = 0;
};
