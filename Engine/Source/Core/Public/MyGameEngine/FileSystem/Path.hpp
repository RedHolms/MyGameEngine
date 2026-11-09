#pragma once

#include <MyGameEngine/String.hpp>
#include <expected>
#include <vector>

/**
 * Represents a path (generally a file path).
 *
 * Note that you can't really construct this object from string. You either need to use `Path::Parse()` function (that
 * can fail). TODO: Make compile-time parser
 */
class Path final {
public:
  static std::expected<Path, String> Parse(String const& pathString);

private:
  explicit constexpr Path() = default;

public:
  String ToString() const;

private:
  enum class PathSeparator {
    // Can only appear inside the first part of a path or as "m_trailingSeparator"
    NONE,

    BACKSLASH,    // `\`
    SLASH         // `/`
  };

  struct PathPart {
    PathSeparator leadingSeparator = PathSeparator::NONE;
    String text;
  };

  char m_driveLetter = 0; // 0 if not set
  std::vector<PathPart> m_parts;
  PathSeparator m_trailingSeparator = PathSeparator::NONE;
};
