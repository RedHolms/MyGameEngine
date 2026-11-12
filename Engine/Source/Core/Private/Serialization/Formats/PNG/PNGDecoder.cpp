#include <MyGameEngine/Serialization/Formats/PNG/PNGDecoder.hpp>

#include <array>
#include <type_traits>
#include <span>

//
// TODO: Move a lot of code to other files (not related to PNG really).
// FIXME: Pretty much whole LZ77 implementation was VIBECODED!!! and also just bad in general. Must be refactored later.
//

template <size_t N>
  requires (N <= 64)
using IntTypeFromBitsN = std::conditional_t<
  (N > 32), uint64, std::conditional_t<
  (N > 16), uint32, std::conditional_t<
  (N > 8), uint16, uint8>>>;

class BitStreamReader {
public:
  explicit constexpr BitStreamReader(Bytes const& source) : m_source(source) {}

public:
  size_t GetNumberOfBitsLeft() const noexcept {
    size_t bytesLeft = m_source.GetSize() - m_bytePtr;
    return (bytesLeft * 8) - m_bitPtr;
  }

  bool ReadBit() {
    uint8_t bit = (m_source.ReadUInt8(m_bytePtr) >> m_bitPtr) & 1;

    if ((++m_bitPtr) == 8) {
      m_bitPtr = 0;
      ++m_bytePtr;
    }

    return bit != 0;
  }

  // Bits are packed least-significant bit first (aligned to the right).
  template <size_t N>
  IntTypeFromBitsN<N> ReadBits() {
    using ResultType = IntTypeFromBitsN<N>;

    ResultType result {};
    for (size_t i = 0; i < N; ++i) {
      result |= static_cast<ResultType>(ReadBit() ? 1 : 0) << i;
    }

    return result;
  }

  // Bits are packed least-significant bit first (aligned to the right).
  uint32_t ReadBitsDynamicUpTo32(uint8_t bitsCount) {
    uint32_t result {};
    for (size_t i = 0; i < bitsCount; ++i) {
      result |= static_cast<uint32_t>(ReadBit() ? 1 : 0) << i;
    }

    return result;
  }

  void AlignToNextByte() {
    if (m_bitPtr != 0) {
      m_bitPtr = 0;
      ++m_bytePtr;
    }
  }

  uint8_t ReadAlignedByte() {
    if (m_bitPtr != 0)
      throw std::runtime_error("Unaligned byte read");

    return m_source.ReadUInt8(m_bytePtr++);
  }

private:
  Bytes const& m_source;
  size_t m_bytePtr = 0;
  uint8_t m_bitPtr = 0;
};

// Canonical Huffman decoder.
//
// Code bits are read in their transmission order (MSB first).
// This is important: DEFLATE's ordinary fields are LSB-first, but Huffman codes themselves are transmitted MSB-first.
class HuffmanTree {
public:
  std::expected<void, String> Build(std::span<uint8_t const> codeLengths, uint maxAllowedBits, bool allowEmpty = false) {
    m_counts.fill(0);
    m_firstCode.fill(0);
    m_firstSymbol.fill(0);
    m_maxBits = 0;

    for (uint8_t length : codeLengths) {
      if (length > maxAllowedBits || length >= m_counts.size())
        return std::unexpected { U"Invalid Huffman code length"_s };

      if (length != 0) {
        ++m_counts[length];

        if (length > m_maxBits) {
          m_maxBits = length;
        }
      }
    }

    if (m_maxBits == 0) {
      m_symbols.clear();

      if (allowEmpty) {
        return {}; // success
      }

      return std::unexpected { U"Empty Huffman tree"_s };
    }

    // Reject oversubscribed Huffman trees.
    // Incomplete trees are allowed; unused codes remain invalid.
    int remaining = 1;

    for (unsigned bits = 1; bits <= maxAllowedBits; ++bits) {
      remaining <<= 1;
      remaining -= m_counts[bits];

      if (remaining < 0) {
        return std::unexpected { U"Oversubscribed Huffman tree"_s };
      }
    }

    // Calculate the first canonical code and symbol offset
    // for each code length.
    uint32_t code = 0;
    uint16_t symbolOffset = 0;

    for (unsigned bits = 1; bits <= maxAllowedBits; ++bits) {
      code = (code + m_counts[bits - 1]) << 1;

      m_firstCode[bits] = code;
      m_firstSymbol[bits] = symbolOffset;

      symbolOffset = static_cast<uint16_t>(
        symbolOffset + m_counts[bits]
      );
    }

    // Store symbols ordered by code length and canonical code.
    m_symbols.resize(symbolOffset);

    std::array<uint16_t, 16> nextSymbol = m_firstSymbol;

    for (uint16_t symbol = 0; symbol < codeLengths.size(); ++symbol) {
      const uint8_t length = codeLengths[symbol];

      if (length != 0) {
        m_symbols[nextSymbol[length]++] = symbol;
      }
    }

    return {}; // Success
  }

  std::expected<uint16_t, String> Decode(BitStreamReader& bits) const {
    if (m_maxBits == 0) {
      return std::unexpected { U"Attempted to decode an empty Huffman tree"_s };
    }

    uint32_t code = 0;

    for (unsigned length = 1; length <= m_maxBits; ++length) {
      // Read Huffman bits in transmission order.
      code = (code << 1) | (bits.ReadBit() ? 1 : 0);

      const uint32_t first = m_firstCode[length];
      const uint32_t count = m_counts[length];

      if (code >= first && code - first < count) {
        return m_symbols[m_firstSymbol[length] + (code - first)];
      }
    }

    return std::unexpected { U"Invalid Huffman code"_s };
  }

private:
  std::array<uint16_t, 16> m_counts {};
  std::array<uint32_t, 16> m_firstCode {};
  std::array<uint16_t, 16> m_firstSymbol {};
  std::vector<uint16_t> m_symbols;
  uint m_maxBits = 0;
};

static inline void EnsureCanAppend(
  std::vector<uint8_t> const& output,
  size_t amount,
  size_t outputLimit
) {
  // FIXME: this func is vibecoded and doesnt go along with my philosophy of minimal exceptions, must be refactored!!

  if (amount > outputLimit - output.size()) {
    throw std::runtime_error("Decompressed data exceeds the output limit");
  }

  if (amount > output.max_size() - output.size()) {
    throw std::runtime_error("Decompressed data exceeds vector::max_size()");
  }
}

static constexpr HuffmanTree MakeFixedLiteralLengthHuffmanTree() {
  std::array<uint8_t, 288> lengths{};

  for (size_t i = 0; i <= 143; ++i) lengths[i] = 8;
  for (size_t i = 144; i <= 255; ++i) lengths[i] = 9;
  for (size_t i = 256; i <= 279; ++i) lengths[i] = 7;
  for (size_t i = 280; i <= 287; ++i) lengths[i] = 8;

  HuffmanTree tree;

  // We might never fail here? Call `value()` just to make sure
  (void) tree.Build(lengths, 15).value();

  return tree;
}

static constexpr HuffmanTree MakeFixedDistanceHuffmanTree() {
  std::array<uint8_t, 32> lengths{};
  lengths.fill(5);

  HuffmanTree tree;

  // We might never fail here? Call `value()` just to make sure
  (void) tree.Build(lengths, 15).value();

  return tree;
}

// Decode one compressed block using its literal/length and distance trees.
static std::expected<void, String> DecodeCompressedBlock(
  BitStreamReader& bits,
  const HuffmanTree& literalLengthTree,
  const HuffmanTree& distanceTree,
  std::vector<uint8_t>& output,
  size_t outputLimit,
  size_t windowSize
) {
  static constexpr std::array<uint16_t, 29> lengthBase = {
    3, 4, 5, 6, 7, 8, 9, 10,
    11, 13, 15, 17,
    19, 23, 27, 31,
    35, 43, 51, 59,
    67, 83, 99, 115,
    131, 163, 195, 227,
    258
  };

  static constexpr std::array<uint8_t, 29> lengthExtra = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2,
    3, 3, 3, 3,
    4, 4, 4, 4,
    5, 5, 5, 5,
    0
  };

  static constexpr std::array<uint16_t, 30> distanceBase = {
    1, 2, 3, 4,
    5, 7, 9, 13,
    17, 25, 33, 49,
    65, 97, 129, 193,
    257, 385, 513, 769,
    1025, 1537, 2049, 3073,
    4097, 6145, 8193, 12289,
    16385, 24577
  };

  static constexpr std::array<uint8_t, 30> distanceExtra = {
    0, 0, 0, 0,
    1, 1, 2, 2,
    3, 3, 4, 4,
    5, 5, 6, 6,
    7, 7, 8, 8,
    9, 9, 10, 10,
    11, 11, 12, 12,
    13, 13
  };

  for (;;) {
    uint16_t symbol;

    if (auto result = literalLengthTree.Decode(bits))
      symbol = result.value();
    else
      return std::unexpected { std::move(result).error() };

    // Literal byte.
    if (symbol < 256) {
      EnsureCanAppend(output, 1, outputLimit);
      output.push_back(static_cast<uint8_t>(symbol));
      continue;
    }

    // End of block.
    if (symbol == 256) {
      return {}; // success
    }

    // Length codes 286 and 287 are reserved.
    if (symbol < 257 || symbol > 285) {
      return std::unexpected { U"Reserved DEFLATE literal/length code"_s };
    }

    // Decode match length.
    const size_t lengthIndex = symbol - 257;

    const size_t length =
      lengthBase[lengthIndex] +
      bits.ReadBitsDynamicUpTo32(lengthExtra[lengthIndex]);

    // Decode match distance.
    uint16_t distanceSymbol;
    if (auto result = distanceTree.Decode(bits))
      distanceSymbol = result.value();
    else
      return std::unexpected { std::move(result).error() };

    if (distanceSymbol >= distanceBase.size()) {
      return std::unexpected { U"Reserved DEFLATE distance code"_s };;
    }

    const size_t distance =
      distanceBase[distanceSymbol] +
      bits.ReadBitsDynamicUpTo32(distanceExtra[distanceSymbol]);

    if (distance > windowSize) {
      return std::unexpected { U"DEFLATE distance exceeds zlib window size"_s };;
    }

    if (distance > output.size()) {
      return std::unexpected { U"DEFLATE distance points before output buffer"_s };;
    }

    EnsureCanAppend(output, length, outputLimit);

    // Overlapping LZ77 copy.
    //
    // Do NOT replace this with memcpy or memmove.
    // For example, distance=1 and length=258 must repeatedly
    // copy the byte that was appended by the previous iteration.
    for (size_t i = 0; i < length; ++i) {
      output.push_back(output[output.size() - distance]);
    }
  }
}

static std::expected<void, String> DecodeDynamicTrees(
  BitStreamReader& bits,
  HuffmanTree& literalLengthTree,
  HuffmanTree& distanceTree
) {
  static constexpr std::array<uint8_t, 19> codeLengthOrder = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5,
    11, 4, 12, 3, 13, 2, 14, 1, 15
  };

  const size_t literalLengthCount = bits.ReadBits<5>() + 257;
  const size_t distanceCount = bits.ReadBits<5>() + 1;
  const size_t codeLengthCount = bits.ReadBits<4>() + 4;

  // HLIT values 30 and 31 are reserved.
  if (literalLengthCount > 286) {
    return std::unexpected { U"Reserved HLIT value"_s };
  }

  // Read code-length alphabet.
  std::array<uint8_t, 19> codeLengthLengths{};

  for (size_t i = 0; i < codeLengthCount; ++i) {
    codeLengthLengths[codeLengthOrder[i]] =
      static_cast<uint8_t>(bits.ReadBits<3>());
  }

  HuffmanTree codeLengthTree;
  if (auto result = codeLengthTree.Build(codeLengthLengths, 7); !result)
    return std::unexpected { std::move(result).error() };

  // Decode literal/length and distance code lengths.
  const size_t total = literalLengthCount + distanceCount;

  std::vector<uint8_t> combinedLengths;
  combinedLengths.reserve(total);

  while (combinedLengths.size() < total) {
    uint16_t symbol;
    if (auto result = codeLengthTree.Decode(bits))
      symbol = result.value();
    else
      return std::unexpected { std::move(result).error() };

    if (symbol <= 15) {
      combinedLengths.push_back(static_cast<uint8_t>(symbol));
      continue;
    }

    size_t repeatCount = 0;
    uint8_t repeatedLength = 0;

    if (symbol == 16) {
      if (combinedLengths.empty()) {
        return std::unexpected { U"Repeat code 16 has no previous length"_s };
      }

      repeatCount = bits.ReadBits<2>() + 3;
      repeatedLength = combinedLengths.back();
    } else if (symbol == 17) {
      repeatCount = bits.ReadBits<3>() + 3;
    } else if (symbol == 18) {
      repeatCount = bits.ReadBits<7>() + 11;
    } else {
      return std::unexpected { U"Invalid code-length symbol"_s };
    }

    if (repeatCount > total - combinedLengths.size()) {
      return std::unexpected { U"Code-length repeat overruns alphabet"_s };
    }

    combinedLengths.insert(
      combinedLengths.end(),
      repeatCount,
      repeatedLength
    );
  }

  std::vector<uint8_t> literalLengthLengths(
    combinedLengths.begin(),
    combinedLengths.begin() + literalLengthCount
  );

  std::vector<uint8_t> distanceLengths(
    combinedLengths.begin() + literalLengthCount,
    combinedLengths.end()
  );

  // End-of-block symbol 256 must be present.
  if (literalLengthLengths[256] == 0) {
    return std::unexpected { U"Missing end-of-block code"_s };
  }

  if (auto result = literalLengthTree.Build(literalLengthLengths, 15); !result)
    return std::unexpected { std::move(result).error() };

  // An empty distance tree is possible when the block contains no
  // length/distance pairs. Decoding an actual distance will fail.
  if (auto result = distanceTree.Build(distanceLengths, 15, true); !result)
    return std::unexpected { std::move(result).error() };
}

std::expected<PNGHeader, String> PNGHeader::DecodeFromChunkContent(Bytes const& chunkContent) noexcept {
  if (chunkContent.GetSize() != 13)
    return std::unexpected { U"Size missmatch"_s };

  uint32 width = chunkContent.ReadUInt32BE(0);
  uint32 height = chunkContent.ReadUInt32BE(4);
  uint8 bitDepth = chunkContent.ReadUInt8(8);
  PNGColorType colorType = static_cast<PNGColorType>(chunkContent.ReadUInt8(9));
  PNGCompressionMethod compressionMethod = static_cast<PNGCompressionMethod>(chunkContent.ReadUInt8(10));
  PNGFilterMethod filterMethod = static_cast<PNGFilterMethod>(chunkContent.ReadUInt8(11));
  PNGInterlaceMethod interlaceMethod = static_cast<PNGInterlaceMethod>(chunkContent.ReadUInt8(12));

  if (width < 0 || width > 0x7FFFFFFF)
    return std::unexpected { U"Invalid image width"_s };
  if (height < 0 || height > 0x7FFFFFFF)
    return std::unexpected { U"Invalid image height"_s };

  switch (colorType) {
    case PNGColorType::GRAYSCALE:
      if (bitDepth != 1 && bitDepth != 2 && bitDepth != 4 && bitDepth != 8 && bitDepth != 16)
        return std::unexpected { U"Invalid bit depth"_s };
      break;

    case PNGColorType::PALETTE:
      if (bitDepth != 1 && bitDepth != 2 && bitDepth != 4 && bitDepth != 8)
        return std::unexpected { U"Invalid bit depth"_s };
      break;

    case PNGColorType::RGB:
    case PNGColorType::GRAYSCALE_ALPHA:
    case PNGColorType::RGBA:
      if (bitDepth != 8 && bitDepth != 16)
        return std::unexpected { U"Invalid bit depth"_s };
      break;

    default:
      return std::unexpected { U"Unknown color type"_s };
  }

  if (compressionMethod != PNGCompressionMethod::INFLATE_DEFLATE)
    return std::unexpected { U"Unknown compression method"_s };

  if (filterMethod != PNGFilterMethod::ADAPTIVE_WITH_5_TYPES)
    return std::unexpected { U"Unknown filter method"_s };

  if (interlaceMethod != PNGInterlaceMethod::NONE && interlaceMethod != PNGInterlaceMethod::ADAM7)
    return std::unexpected { U"Unknown interlace method"_s };

  return PNGHeader(Init {
    .width = width,
    .height = height,
    .bitDepth = bitDepth,
    .colorType = colorType,
    .compressionMethod = compressionMethod,
    .filterMethod = filterMethod,
    .interlaceMethod = interlaceMethod
  });
}

PNGDecoder::PNGDecoder(PNGHeader const& header)
  : m_header(header) {}

void PNGDecoder::AppendDataChunk(Bytes const& chunkContent) {
  if (chunkContent.GetSize() == 0)
    return;

  size_t previousSize = m_pngData.GetSize();
  m_pngData.Resize(previousSize + chunkContent.GetSize());

  MemoryUtils::CopyBytes(
    static_cast<uint8*>(m_pngData.GetBuffer()) + previousSize,
    chunkContent.GetData(),
    chunkContent.GetSize());
}

std::expected<DecodedImage, String> PNGDecoder::Decode() const {
  Buffer decompressedData;
  if (auto result = Decompress()) {
    decompressedData = std::move(result).value();
  }
  else {
    return std::unexpected { std::move(result).error() };
  }

}

std::expected<Buffer, String> PNGDecoder::Decompress() const {
  // Minimum: 2-byte zlib header + at least 0-byte DEFLATE
  // payload (in practice at least one block byte) + 4-byte checksum.
  if (m_pngData.GetSize() < 6) {
    return std::unexpected { U"Truncated zlib stream"_s };
  }

  // Parse zlib header (RFC 1950).
  const uint8_t cmf = m_pngData.ReadUInt8(0);
  const uint8_t flg = m_pngData.ReadUInt8(1);

  // Compression method must be DEFLATE (8).
  if ((cmf & 0x0F) != 8) {
    return std::unexpected { U"Zlib stream does not use DEFLATE"_s };
  }

  // CINFO must be <= 7 (maximum window size: 32768 bytes).
  const unsigned cinfo = cmf >> 4;

  if (cinfo > 7) {
    return std::unexpected { U"Invalid zlib window size"_s };
  }

  // FCHECK: the 16-bit header must be divisible by 31.
  if (((static_cast<unsigned>(cmf) << 8) | flg) % 31 != 0) {
    return std::unexpected { U"Invalid zlib FCHECK"_s };
  }

  // PNG does not permit preset dictionaries.
  if ((flg & 0x20) != 0) {
    return std::unexpected { U"Preset zlib dictionaries are not supported"_s };
  }

  const size_t windowSize = size_t{1} << (cinfo + 8);

  // Skip the two-byte zlib header and four-byte Adler-32 trailer.
  auto compressedStreamView = m_pngData.GetView(2, m_pngData.GetSize() - 2 - 4);
  BitStreamReader bits(compressedStreamView);

  std::vector<uint8_t> output;

  bool finalBlock = false;

  while (!finalBlock) {
    finalBlock = bits.ReadBits<1>() != 0;

    const uint32_t blockType = bits.ReadBits<2>();

    switch (blockType) {
      case 0: {
        // Stored (uncompressed) block.
        bits.AlignToNextByte();

        const uint16_t length =
          static_cast<uint16_t>(bits.ReadAlignedByte()) |
          (static_cast<uint16_t>(bits.ReadAlignedByte()) << 8);

        const uint16_t inverseLength =
          static_cast<uint16_t>(bits.ReadAlignedByte()) |
          (static_cast<uint16_t>(bits.ReadAlignedByte()) << 8);

        if (static_cast<uint16_t>(length ^ 0xFFFFu) != inverseLength) {
          return std::unexpected { U"Invalid stored block LEN/NLEN"_s };
        }

        EnsureCanAppend(output, length, std::numeric_limits<size_t>::max());

        for (uint32_t i = 0; i < length; ++i) {
          output.push_back(bits.ReadAlignedByte());
        }

        break;
      }

      case 1: {
        // Fixed Huffman block.
        const auto literalLengthTree = MakeFixedLiteralLengthHuffmanTree();
        const auto distanceTree = MakeFixedDistanceHuffmanTree();

        if (auto result = DecodeCompressedBlock(
          bits,
          literalLengthTree,
          distanceTree,
          output,
          std::numeric_limits<size_t>::max(),
          windowSize
        ); !result) {
          return std::unexpected { std::move(result).error() };
        }

        break;
      }

      case 2: {
        // Dynamic Huffman block.
        HuffmanTree literalLengthTree;
        HuffmanTree distanceTree;

        if (auto result = DecodeDynamicTrees(
          bits,
          literalLengthTree,
          distanceTree
        ); !result) {
          return std::unexpected { std::move(result).error() };
        }

        if (auto result = DecodeCompressedBlock(
          bits,
          literalLengthTree,
          distanceTree,
          output,
          std::numeric_limits<size_t>::max(),
          windowSize
        ); !result) {
          return std::unexpected { std::move(result).error() };
        }

        break;
      }

      default:
        // BTYPE = 11 is reserved.
        return std::unexpected { U"Reserved DEFLATE block type"_s };
    }
  }

  // The final DEFLATE block ends at the next byte boundary.
  bits.AlignToNextByte();

  if (bits.GetNumberOfBitsLeft() != 0) {
    return std::unexpected { U"Unexpected bytes after final DEFLATE block"_s };
  }

  //
  // PLEASE FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // This is vibecoded and ChatGPT really wants to use vector of bytes (i didnt tell them about my Buffer stuff). I
  // SWEAR!! I will rewrite it properly when I'll move compression stuff to its own files/classes. Anyway I was going to
  // use LZ77 for assets compression.
  //
  // im lazy.
  //

  Buffer decompressedBuffer = Buffer::AllocateGarbaged(output.size());
  MemoryUtils::CopyBytes(
    decompressedBuffer.GetBuffer(),
    output.data(),
    output.size());

  return decompressedBuffer;
}
