#pragma once

#include <MyGameEngine/Bytes.hpp>
#include <MyGameEngine/PImpl.hpp>
#include <MyGameEngine/String.hpp>
#include <expected>
#include <filesystem>

// TODO: Maybe make IAssetsManager and impls like SyncAssetsManager and AsyncAssetsManager
//  we're doing only sync for now cuz im lazzzyyyyyyyy asf

class AssetsManager {
public:
  AssetsManager();
  ~AssetsManager();

  AssetsManager(AssetsManager const&) = delete;
  AssetsManager(AssetsManager&&) = delete;
  AssetsManager& operator=(AssetsManager const&) = delete;
  AssetsManager& operator=(AssetsManager&&) = delete;

public:
  std::expected<void, String> LoadArchive(std::filesystem::path const& archiveFilePath);

  std::expected<Buffer, String> GetAssetData(String const& assetName);

private:
  struct Impl; PImpl<Impl, 144> m;
};
