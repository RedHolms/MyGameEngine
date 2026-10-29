#include <MyGameEngine/Assets/AssetsManager.hpp>

#include <deque>
#include <map>
#include <unordered_map>
#include <set>

#include <MyGameEngine/Win32Inc.hpp>

using ArchiveID = umword_t;
using AssetID = umword_t;

constexpr uint8 A4PF_CURRENT_FORMAT_VERSION = 1;

namespace {

struct AssetInformation {
  String name;
  uint64_t begin; // in bytes
  uint64_t size; // in bytes
  ArchiveID archiveId;
};

struct ArchiveState {
  HANDLE fileHandle;
  std::set<AssetID> assetIds;

  explicit ArchiveState(HANDLE fileHandle) : fileHandle(fileHandle) {}

  ArchiveState(ArchiveState&& other) noexcept : fileHandle(other.fileHandle),
    assetIds(std::move(other.assetIds))
  {
    other.fileHandle = nullhandle;
  }

  ArchiveState& operator=(ArchiveState&& other) noexcept {
    ::CloseHandle(fileHandle);
    fileHandle = other.fileHandle;
    assetIds = std::move(other.assetIds);
    other.fileHandle = nullhandle;
    return *this;
  }

  ~ArchiveState() {
    ::CloseHandle(fileHandle);
  }
};

} // namespace <anonymous>

struct AssetsManager::Impl {
  std::map<ArchiveID, ArchiveState> m_archives {};
  ArchiveID m_firstFreeArchiveId = 1;

  std::map<AssetID, AssetInformation> m_assets {};
  AssetID m_firstFreeAssetId = 1;

  std::unordered_map<String, AssetID> m_assetName2IdMap;

  std::expected<void, String> LoadArchive(std::filesystem::path const& archiveFilePath) {
    auto const filePathWStr = archiveFilePath.wstring();

    ::HANDLE fileHandle = ::CreateFileW(
      filePathWStr.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullhandle);

    if (fileHandle == nullhandle)
      return std::unexpected { U"::CreateFileW() failed"_s }; // TODO: Format win32 error

    ArchiveID id = m_firstFreeArchiveId;
    ArchiveState state { fileHandle };

    if (auto result = ParseArchive(id, state); !result)
      return std::unexpected { std::move(result).error() };

    ++m_firstFreeArchiveId;
    m_archives.emplace(id, std::move(state));

    return {}; // success
  }

  std::expected<Buffer, String> GetAssetData(String const& assetName) {
    auto it = m_assetName2IdMap.find(assetName);

    if (it == m_assetName2IdMap.end())
      return std::unexpected { U"Asset with given name not found"_s };

    auto assetId = it->second;
    auto const& asset = m_assets.at(assetId);

    auto archiveId = asset.archiveId;
    auto const& archive = m_archives.at(archiveId);

    auto buffer = Buffer::AllocateGarbaged(asset.size);

    LARGE_INTEGER filePos {};
    filePos.QuadPart = asset.begin;
    ::SetFilePointerEx(archive.fileHandle, filePos, nullptr, FILE_BEGIN);

    DWORD bytesRead;
    BOOL success = ::ReadFile(archive.fileHandle, buffer.GetBuffer(), asset.size, &bytesRead, nullptr);

    if (!success) {
      return std::unexpected { U"::ReadFile() failed"_s }; // todo: format win32 error
    }

    if (bytesRead != asset.size) {
      return std::unexpected { U"Could not read enough bytes"_s };
    }

    return std::expected<Buffer, String> { std::move(buffer) };
  }

  std::expected<void, String> ParseArchive(ArchiveID id, ArchiveState& state) {
    auto handle = state.fileHandle;

    ::DWORD read;
    char signature[4];
    uint8 formatVersion;
    uint32 filesCount;
    uint64 stringsTableOffset;
    uint32 stringsCount;
    uint32 stringLength;
    std::vector<uint8> stringBuffer;
    std::vector<String> stringsTable;
    uint32 fileNameStrIndex;
    uint64 fileContentSize;
    uint64 fileBegin;
    ::LARGE_INTEGER tempLargeInt;

    // TODO: Create some kind of "FileByteStream" and in general implement tools for serialization and stuff

    auto readBytesChecked = [&](void* dest, size_t count, String&& notEnoughString) -> std::optional<String> {
      if (!::ReadFile(handle, dest, count, &read, nullptr)) {
        // TODO: Format win32 error
        return U"::ReadFile() failed"_s;
      }

      if (read < count)
        // is move necessary here? im new to cpp yaknow. wouldn't hurt anyway ig
        return std::move(notEnoughString);

      return std::nullopt;
    };

    if (auto error = readBytesChecked(&signature[0], 4, U"File doesn't contain signature"_s)) {
      return std::unexpected { std::move(error).value() };
    }

    if (strncmp(signature, "A4PF", 4) != 0) {
      return std::unexpected { U"File has an invalid signature"_s };
    }

    if (auto error = readBytesChecked(&formatVersion, 1, U"File doesn't contain format version"_s)) {
      return std::unexpected { std::move(error).value() };
    }

    if (formatVersion != A4PF_CURRENT_FORMAT_VERSION) {
      return std::unexpected { U"File's format version is unsupported"_s };
    }

    if (auto error = readBytesChecked(&filesCount, 4, U"File doesn't contain files count"_s)) {
      return std::unexpected { std::move(error).value() };
    }

    if (auto error = readBytesChecked(&stringsTableOffset, 8, U"File doesn't contain strings table offset"_s)) {
      return std::unexpected { std::move(error).value() };
    }

    //
    // Read strings table
    //

    tempLargeInt.QuadPart = stringsTableOffset;
    ::SetFilePointerEx(handle, tempLargeInt, nullptr, FILE_BEGIN);

    if (auto error = readBytesChecked(&stringsCount, 4, U"File doesn't contain strings count in the strings table"_s)) {
      return std::unexpected { std::move(error).value() };
    }

    for (size_t i = 0; i < stringsCount; ++i) {
      if (auto error = readBytesChecked(&stringLength, 4, U"File doesn't contain string length in the strings table"_s)) {
        return std::unexpected { std::move(error).value() };
      }

      stringBuffer.resize(stringLength);
      if (auto error = readBytesChecked(stringBuffer.data(), stringLength, U"File doesn't contain string data in the strings table"_s)) {
        return std::unexpected { std::move(error).value() };
      }

      stringsTable.emplace_back(String::Decode(Bytes { stringBuffer.data(), stringBuffer.size() }, Encoding::UTF8));
    }

    //
    // Read asset descriptions
    //

    // FIXME hardcoded offset
    ::SetFilePointer(handle, 17, nullptr, FILE_BEGIN);

    for (size_t i = 0; i < filesCount; ++i) {
      if (auto error = readBytesChecked(&fileNameStrIndex, 4, U"File doesn't contain asset name index"_s)) {
        return std::unexpected { std::move(error).value() };
      }

      if (fileNameStrIndex >= stringsTable.size()) {
        return std::unexpected { U"Asset has an invalid asset name index"_s };
      }

      String const& fileName = stringsTable[fileNameStrIndex];

      if (auto error = readBytesChecked(&fileContentSize, 8, U"File doesn't contain asset content size"_s)) {
        return std::unexpected { std::move(error).value() };
      }

      if (auto error = readBytesChecked(&fileBegin, 8, U"File doesn't contain asset data begin offsetx"_s)) {
        return std::unexpected { std::move(error).value() };
      }

      // fuck naming consistency!
      auto assetId = m_firstFreeAssetId++;

      state.assetIds.emplace(assetId);
      m_assetName2IdMap.emplace(fileName, assetId);
      m_assets.emplace(assetId, AssetInformation {
        .name = fileName,
        .begin = fileBegin,
        .size = fileContentSize,
        .archiveId = id
      });
    }

    return {}; // success
  }
};

AssetsManager::AssetsManager() = default;
AssetsManager::~AssetsManager() = default;

std::expected<void, String> AssetsManager::LoadArchive(std::filesystem::path const& archiveFilePath) {
  return m->LoadArchive(archiveFilePath);
}

std::expected<Buffer, String> AssetsManager::GetAssetData(String const& assetName) {
  return m->GetAssetData(assetName);
}
