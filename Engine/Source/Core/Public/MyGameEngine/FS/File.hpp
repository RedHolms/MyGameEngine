#pragma once

#include <MyGameEngine/DataSource.hpp>
#include <fstream>
#include <mutex>

// TODO: Make our own file class

class SyncFileByteSource final : public ISyncRandomAccessByteSource {
public:
  explicit constexpr SyncFileByteSource(std::ifstream&& file) : m_file(std::move(file)) {}

  ~SyncFileByteSource() override = default;

public:
  inline size_t GetDataSize() const override {
    // Can file size change while its opened? I think yes

    std::lock_guard readingLockGuard { m_readingMutex };
    m_file.seekg(0, std::ios::end);
    return m_file.tellg();
  }

  inline bool IsPlacedInMemory() const override {
    return false;
  }

  inline std::expected<void, String> ReadBytesTo(size_t offset, void* outBuffer, size_t bytesCount) const override {
    if (bytesCount == 0)
      return {}; // success

    std::lock_guard readingLockGuard { m_readingMutex };

    m_file.seekg(offset, std::ios::beg);
    m_file.read(static_cast<char*>(outBuffer), bytesCount);

    if (m_file.eof())
      return std::unexpected { U"End of file reached"_s };

    if (m_file.fail())
      return std::unexpected { U"Failed to read from file"_s };

    return {}; // success
  }

private:
  mutable std::ifstream m_file;
  mutable std::mutex m_readingMutex;
};
