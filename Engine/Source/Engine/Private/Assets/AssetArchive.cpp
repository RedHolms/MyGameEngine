#include <MyGameEngine/Assets/AssetArchive.hpp>
#include <algorithm>
#include <fstream>
#include <limits>

namespace {

bool IsValidArchivePath(std::string_view path) {
  if (path.empty() || path.front() == '/' || path.find('\\') != std::string_view::npos ||
      path.find(':') != std::string_view::npos || path.find('\0') != std::string_view::npos)
    return false;

  size_t segmentBegin = 0;
  while (segmentBegin <= path.size()) {
    size_t const segmentEnd = path.find('/', segmentBegin);
    std::string_view const segment = path.substr(segmentBegin,
      segmentEnd == std::string_view::npos ? path.size() - segmentBegin : segmentEnd - segmentBegin);
    if (segment.empty() || segment == "." || segment == "..")
      return false;
    if (segmentEnd == std::string_view::npos)
      break;
    segmentBegin = segmentEnd + 1;
  }
  return true;
}

bool IsValidUtf8(std::string_view text) {
  size_t index = 0;
  while (index < text.size()) {
    uint8 const first = static_cast<uint8>(text[index]);
    if (first <= 0x7f) {
      ++index;
      continue;
    }

    size_t continuationCount;
    uint32 codepoint;
    if (first >= 0xc2 && first <= 0xdf) {
      continuationCount = 1;
      codepoint = first & 0x1f;
    }
    else if (first >= 0xe0 && first <= 0xef) {
      continuationCount = 2;
      codepoint = first & 0x0f;
    }
    else if (first >= 0xf0 && first <= 0xf4) {
      continuationCount = 3;
      codepoint = first & 0x07;
    }
    else {
      return false;
    }

    if (continuationCount > text.size() - index - 1)
      return false;
    for (size_t continuationIndex = 1; continuationIndex <= continuationCount; ++continuationIndex) {
      uint8 const next = static_cast<uint8>(text[index + continuationIndex]);
      if ((next & 0xc0) != 0x80)
        return false;
      codepoint = (codepoint << 6) | (next & 0x3f);
    }
    if ((continuationCount == 1 && codepoint < 0x80) ||
        (continuationCount == 2 && codepoint < 0x800) ||
        (continuationCount == 3 && codepoint < 0x10000) ||
        (codepoint >= 0xd800 && codepoint <= 0xdfff) || codepoint > 0x10ffff)
      return false;
    index += continuationCount + 1;
  }
  return true;
}

uint32 ReadUInt32LE(uint8 const* data) {
  return static_cast<uint32>(data[0]) |
    (static_cast<uint32>(data[1]) << 8) |
    (static_cast<uint32>(data[2]) << 16) |
    (static_cast<uint32>(data[3]) << 24);
}

uint64 ReadUInt64LE(uint8 const* data) {
  uint64 value = 0;
  for (uint index = 0; index < 8; ++index)
    value |= static_cast<uint64>(data[index]) << (index * 8);
  return value;
}

} // namespace

std::expected<AssetArchive, std::string> AssetArchive::Open(std::filesystem::path const& path) {
  std::ifstream stream(path, std::ios::binary | std::ios::ate);
  if (!stream)
    return std::unexpected("Unable to open asset archive: " + path.string());

  std::streampos const end = stream.tellg();
  if (end < 0 || static_cast<uint64>(end) > std::numeric_limits<size_t>::max() ||
      static_cast<uint64>(end) > static_cast<uint64>(std::numeric_limits<std::streamsize>::max()))
    return std::unexpected("Asset archive is too large: " + path.string());

  AssetArchive archive;
  archive.m_data.resize(static_cast<size_t>(end));
  stream.seekg(0, std::ios::beg);
  if (!archive.m_data.empty() && !stream.read(reinterpret_cast<char*>(archive.m_data.data()),
        static_cast<std::streamsize>(archive.m_data.size())))
    return std::unexpected("Unable to read asset archive: " + path.string());

  auto const* data = archive.m_data.data();
  size_t const dataSize = archive.m_data.size();
  if (dataSize < 4)
    return std::unexpected("Asset archive header is truncated: " + path.string());

  uint32 const assetCount = ReadUInt32LE(data);
  if (assetCount > (dataSize - 4) / 20)
    return std::unexpected("Asset archive entry table is truncated: " + path.string());
  size_t cursor = 4;
  for (uint32 index = 0; index < assetCount; ++index) {
    if (cursor > dataSize || dataSize - cursor < 4)
      return std::unexpected("Asset archive entry header is truncated: " + path.string());

    uint32 const nameSize = ReadUInt32LE(data + cursor);
    cursor += 4;
    if (nameSize > dataSize - cursor || dataSize - cursor - nameSize < 16)
      return std::unexpected("Asset archive entry is truncated: " + path.string());

    std::string name(reinterpret_cast<char const*>(data + cursor), nameSize);
    cursor += nameSize;
    uint64 const assetOffset = ReadUInt64LE(data + cursor);
    uint64 const assetSize = ReadUInt64LE(data + cursor + 8);
    cursor += 16;

    if (!IsValidUtf8(name) || !IsValidArchivePath(name))
      return std::unexpected("Asset archive contains an invalid asset path: " + name);
    if (assetOffset > dataSize || assetSize > dataSize - static_cast<size_t>(assetOffset))
      return std::unexpected("Asset archive contains an out-of-range asset: " + name);
    if (archive.m_assets.contains(name))
      return std::unexpected("Asset archive contains a duplicate asset path: " + name);

    archive.m_assets.emplace(std::move(name), AssetRange {
      static_cast<size_t>(assetOffset), static_cast<size_t>(assetSize)
    });
  }

  std::vector<AssetRange> ranges;
  ranges.reserve(archive.m_assets.size());
  for (auto const& [name, range] : archive.m_assets) {
    (void) name;
    if (range.offset < cursor)
      return std::unexpected("Asset archive asset overlaps its header: " + path.string());
    ranges.push_back(range);
  }
  std::ranges::sort(ranges, {}, &AssetRange::offset);
  for (size_t index = 1; index < ranges.size(); ++index) {
    AssetRange const& previous = ranges[index - 1];
    if (ranges[index].offset < previous.offset + previous.size)
      return std::unexpected("Asset archive contains overlapping asset data: " + path.string());
  }

  return archive;
}

std::optional<Bytes> AssetArchive::Find(std::string_view archivePath) const {
  std::string const key(archivePath);
  auto const found = m_assets.find(key);
  if (found == m_assets.end())
    return std::nullopt;
  return Bytes { m_data.data() + found->second.offset, found->second.size };
}

std::expected<void, std::string> AssetManager::MountArchive(std::filesystem::path const& path) {
  auto archive = AssetArchive::Open(path);
  if (!archive)
    return std::unexpected(archive.error());
  m_archives.push_back(std::move(*archive));
  return {};
}

std::optional<Bytes> AssetManager::Find(std::string_view archivePath) const {
  for (auto archive = m_archives.rbegin(); archive != m_archives.rend(); ++archive) {
    if (auto asset = archive->Find(archivePath))
      return asset;
  }
  return std::nullopt;
}
