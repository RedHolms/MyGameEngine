#pragma once

#include <MyGameEngine/String.hpp>
#include <expected>

enum class FileOpenMode {
  READ,
  WRITE
};

struct FileOpenProperties {
  // Allows other processes to delete this file while it's opened
  bool allowDelete = false;

  // Allows other processes to read from this file while it's opened
  bool allowRead = true;

  // Allows other processes to write to this file while it's opened
  bool allowWrite = false;
};

/**
 * This class represents a file opened for synchronous operations.
 */
class SyncFile {
public:
  static std::expected<SyncFile, String> Open(String const& filePath, FileOpenMode mode,
    FileOpenProperties properties = {});

public:
  std::expected<size_t, String> GetContentSize();

  /**
   * NOTE: This function succeeds even if @c offset points beyond the end of the file (after EOF). In this case, @c 0 is
   * returned.
   *
   * @return Amount of bytes read
   */
  std::expected<size_t, String> Read(size_t offset, void* outBuffer, size_t bytesCount);

  /**
   * @return Amount of bytes written. Might be @c 0
   */
  std::expected<size_t, String> Write(size_t offset, void const* data, size_t bytesCount);
};
