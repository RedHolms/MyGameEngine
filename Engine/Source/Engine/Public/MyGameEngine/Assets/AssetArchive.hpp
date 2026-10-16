#pragma once

#include <MyGameEngine/Bytes.hpp>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/// A loaded .mga archive. Asset byte views remain valid while this archive remains alive.
class AssetArchive final {
public:
  static std::expected<AssetArchive, std::string> Open(std::filesystem::path const& path);

  /// Returns a non-owning view of an asset, or an empty optional when the path is absent.
  [[nodiscard]] std::optional<Bytes> Find(std::string_view archivePath) const;

private:
  struct AssetRange {
    size_t offset;
    size_t size;
  };

  std::vector<uint8> m_data;
  std::unordered_map<std::string, AssetRange> m_assets;
};

/// Owns mounted archives and resolves assets from the most recently mounted archive first.
class AssetManager final {
public:
  std::expected<void, std::string> MountArchive(std::filesystem::path const& path);

  /// Returns a non-owning view. It remains valid until this manager is destroyed.
  [[nodiscard]] std::optional<Bytes> Find(std::string_view archivePath) const;

private:
  std::vector<AssetArchive> m_archives;
};
