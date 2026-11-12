#pragma once

#include <MyGameEngine/Bytes.hpp>
#include <ranges>

struct PNGChunkType {
  char type[4];

  constexpr PNGChunkType(char c1, char c2, char c3, char c4) noexcept : type{ c1, c2, c3, c4 } {}

  bool IsCritical() const noexcept {
    return (type[0] & (1 << 5)) == 0;
  }

  bool IsAncillary() const noexcept {
    return (type[0] & (1 << 5)) == 1;
  }

  bool IsPublic() const noexcept {
    return (type[1] & (1 << 5)) == 0;
  }

  bool IsPrivate() const noexcept {
    return (type[1] & (1 << 5)) == 1;
  }

  bool IsSafeToCopy() const noexcept {
    return (type[3] & (1 << 5)) == 1;
  }

  constexpr bool operator==(PNGChunkType const& other) const noexcept {
    return std::bit_cast<uint32>(type) == std::bit_cast<uint32>(other.type);
  }
};

namespace PNGKnownChunkTypes {

//
// Critical chunks
//

constexpr PNGChunkType IHDR { 'I', 'H', 'D', 'R' };
constexpr PNGChunkType PLTE { 'P', 'L', 'T', 'E' };
constexpr PNGChunkType IDAT { 'I', 'D', 'A', 'T' };
constexpr PNGChunkType IEND { 'I', 'E', 'N', 'D' };

//
// Ancillary chunks
//

constexpr PNGChunkType cHRM { 'c', 'H', 'R', 'M' };
constexpr PNGChunkType gAMA { 'g', 'A', 'M', 'A' };
constexpr PNGChunkType iCCP { 'i', 'C', 'C', 'P' };
constexpr PNGChunkType sBIT { 's', 'B', 'I', 'T' };
constexpr PNGChunkType sRGB { 's', 'R', 'G', 'B' };
constexpr PNGChunkType bKGD { 'b', 'K', 'G', 'D' };
constexpr PNGChunkType hIST { 'h', 'I', 'S', 'T' };
constexpr PNGChunkType tRNS { 't', 'R', 'N', 'S' };
constexpr PNGChunkType pHYs { 'p', 'H', 'Y', 's' };
constexpr PNGChunkType sPLT { 's', 'P', 'L', 'T' };
constexpr PNGChunkType tIME { 't', 'I', 'M', 'E' };
constexpr PNGChunkType iTXt { 'i', 'T', 'X', 't' };
constexpr PNGChunkType tEXt { 't', 'E', 'X', 't' };
constexpr PNGChunkType zTXt { 'z', 'T', 'X', 't' };

} // namespace PNGKnownChunkTypes

struct PNGChunk {
  PNGChunkType type;
  Buffer content;

  bool IsCritical() const noexcept { return type.IsCritical(); }
  bool IsAncillary() const noexcept { return type.IsAncillary(); }
  bool IsPublic() const noexcept { return type.IsPublic(); }
  bool IsPrivate() const noexcept { return type.IsPrivate(); }
  bool IsSafeToCopy() const noexcept { return type.IsSafeToCopy(); }
};

struct PNGFile {
  static std::expected<PNGFile, String> ParseFile(Bytes const& fileBytes);

  std::vector<PNGChunk> chunks;

  inline auto FindAllChunkByType(PNGChunkType const& type) const noexcept {
    return chunks
      | std::ranges::views::filter([wantedType=type](PNGChunk const& chunk) { return chunk.type == wantedType; });
  }
};
