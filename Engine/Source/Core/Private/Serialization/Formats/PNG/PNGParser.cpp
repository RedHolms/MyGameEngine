#include <MyGameEngine/Serialization/Formats/PNG/PNGParser.hpp>

#include <array>

constexpr uint8_t PNG_SIGNATURE[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

// TODO: Move CRC computation code to somewhere else
constexpr std::array<uint8, 256> CRC_TABLE = [] {
  std::array<uint8, 256> result {};

  for (uint n = 0; n < 256; ++n) {
    uint32 c = n;

    for (uint k = 0; k < 8; ++k) {
      if (c & 1)
        c = 0xedb88320L ^ (c >> 1);
      else
        c = c >> 1;
    }

    result[n] = c;
  }

  return result;
}();

std::expected<PNGFile, String> PNGFile::ParseFile(Bytes const& fileBytes) {
  if (fileBytes.GetSize() < std::size(PNG_SIGNATURE))
    return std::unexpected { U"Invalid PNG signature"_s };

  for (size_t i = 0; i < std::size(PNG_SIGNATURE); ++i) {
    if (fileBytes.ReadUInt8(i) != PNG_SIGNATURE[i])
      return std::unexpected { U"Invalid PNG signature"_s };
  }

  size_t ptr = std::size(PNG_SIGNATURE);

  PNGFile result {};

  //
  // Read the chunks
  //

  while (ptr < fileBytes.GetSize()) {
    // We expect at least 12 bytes here (chunk length, type and CRC)

    if (ptr + 12 > fileBytes.GetSize())
      return std::unexpected { U"Unexpected end of data"_s };

    uint32_t chunkLength = fileBytes.ReadUInt32BE(ptr);
    char chunkTypeC1 = static_cast<char>(fileBytes.ReadUInt8(ptr+4));
    char chunkTypeC2 = static_cast<char>(fileBytes.ReadUInt8(ptr+5));
    char chunkTypeC3 = static_cast<char>(fileBytes.ReadUInt8(ptr+6));
    char chunkTypeC4 = static_cast<char>(fileBytes.ReadUInt8(ptr+7));
    ptr += 8;

    if (ptr + chunkLength + 4 > fileBytes.GetSize())
      return std::unexpected { U"Unexpected end of data"_s };

    uint32_t chunkComputedCRC = 0xFFFFFFFF;
    for (size_t i = 0; i < chunkLength; ++i)
      chunkComputedCRC = CRC_TABLE[(chunkComputedCRC ^ fileBytes.ReadUInt8(ptr+i)) & 0xff] ^ (chunkComputedCRC >> 8);
    chunkComputedCRC ^= 0xFFFFFFFF;

    uint32_t chunkExpectedCRC = fileBytes.ReadUInt32BE(ptr + chunkLength);

    if (chunkComputedCRC != chunkExpectedCRC)
      return std::unexpected { U"CRC missmatch"_s };

    Buffer chunkContent = Buffer::AllocateGarbaged(chunkLength);
    MemoryUtils::CopyBytes(
      chunkContent.GetBuffer(),
      static_cast<uint8 const*>(fileBytes.GetData()) + ptr,
      chunkLength);

    result.chunks.emplace_back(
      PNGChunkType { chunkTypeC1, chunkTypeC2, chunkTypeC3, chunkTypeC4 },
      std::move(chunkContent)
    );
  }

  return result;
}