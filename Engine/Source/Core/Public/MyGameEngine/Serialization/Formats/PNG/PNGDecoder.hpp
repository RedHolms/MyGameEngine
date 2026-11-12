#pragma once

#include <MyGameEngine/Bytes.hpp>
#include <MyGameEngine/IntTypes.hpp>
#include <expected>

enum class PNGColorType : uint8 {
  FLAG_PALETTE_USED = 1 << 0,
  FLAG_COLOR_USED   = 1 << 1,
  FLAG_ALPHA_USED   = 1 << 2,

  GRAYSCALE       = 0,
  RGB             = FLAG_COLOR_USED,
  PALETTE         = FLAG_PALETTE_USED | FLAG_COLOR_USED,
  GRAYSCALE_ALPHA = FLAG_ALPHA_USED,
  RGBA            = FLAG_COLOR_USED | FLAG_ALPHA_USED
};

enum class PNGCompressionMethod : uint8 {
  INFLATE_DEFLATE = 0
};

enum class PNGFilterMethod : uint8 {
  ADAPTIVE_WITH_5_TYPES = 0
};

enum class PNGInterlaceMethod : uint8 {
  NONE = 0,
  ADAM7 = 1
};

// Structure that holds information from IHDR section
struct PNGHeader {
public:
  static std::expected<PNGHeader, String> DecodeFromChunkContent(Bytes const& chunkContent) noexcept;

private:
  struct Init {
    uint32 width;
    uint32 height;
    uint8 bitDepth;
    PNGColorType colorType;
    PNGCompressionMethod compressionMethod;
    PNGFilterMethod filterMethod;
    PNGInterlaceMethod interlaceMethod;
  };

  explicit(false) constexpr PNGHeader(Init&& init) noexcept
    : m_width(init.width),
      m_height(init.height),
      m_bitDepth(init.bitDepth),
      m_colorType(init.colorType),
      m_compressionMethod(init.compressionMethod),
      m_filterMethod(init.filterMethod),
      m_interlaceMethod(init.interlaceMethod) {}

public:
  constexpr ~PNGHeader() noexcept = default;

public:
  uint32 GetWidth() const noexcept {
    return m_width;
  }

  uint32 GetHeight() const noexcept {
    return m_height;
  }

  uint8 GetBitDepth() const noexcept {
    return m_bitDepth;
  }

  PNGColorType GetColorType() const noexcept {
    return m_colorType;
  }

  PNGCompressionMethod GetCompressionMethod() const noexcept {
    return m_compressionMethod;
  }

  PNGFilterMethod GetFilterMethod() const noexcept {
    return m_filterMethod;
  }

  PNGInterlaceMethod GetInterlaceMethod() const noexcept {
    return m_interlaceMethod;
  }

private:
  uint32 m_width;
  uint32 m_height;
  uint8 m_bitDepth;
  PNGColorType m_colorType;
  PNGCompressionMethod m_compressionMethod;
  PNGFilterMethod m_filterMethod;
  PNGInterlaceMethod m_interlaceMethod;
};

// FIXME: Either move it somewhere else or rename to something PNG-specific (DecodedPNG?)
struct DecodedImage {
  uint width;
  uint height;

  /**
   * Buffer contains RGBA pixels. Each pixel is 16 bytes wide and consists of 4 consecutive 8 bit numbers: Red, Green,
   * Blue and Alpha. Size of the meaningful data in the buffer is guaranteed to be `width * height * 16` bytes. Although
   * actual buffer might be bigger.
   */
  Buffer pixelData;
};

class PNGDecoder {
public:
  explicit PNGDecoder(PNGHeader const& header);

public:
  void AppendDataChunk(Bytes const& chunkContent);

  std::expected<DecodedImage, String> Decode() const;

private:
  std::expected<Buffer, String> Decompress() const;

private:
  PNGHeader m_header;
  Buffer m_pngData;
};
