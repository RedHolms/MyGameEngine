#include <MyGameEngine/FileSystem/Path.hpp>

static bool isPathSeparator(char32_t character) {
  return character == '\\' || character == '/';
}

std::expected<Path, String> Path::Parse(String const& pathString) {
  if (pathString.IsEmpty())
    return std::unexpected { U"Empty path is not acceptable"_s };

  if (pathString.GetLength() == 1) {
    if (pathString[0] < 32)
      return std::unexpected { U"Illegal characters"_s };

    switch (pathString[0]) {
      case '<':
      case '>':
      case ':':
      case '\"':
      case '|':
      case '?':
      case '*':
      case '\0':
        return std::unexpected { U"Illegal characters"_s };

      case '\\':
      case '/': {
        // Empty absolute path
        Path result {};
        result.m_trailingSeparator = pathString[0] == '/' ? PathSeparator::SLASH : PathSeparator::BACKSLASH;
        return result;
      }

      default: {
        Path result {};
        result.m_parts.push_back(PathPart { .text = pathString });
        return result;
      }
    }
  }

  Path result {};
  size_t index = 0;

  if (pathString[1] == ':') {
    // Path must contain drive letter

    if (pathString.GetLength() < 3)
      return std::unexpected { U"Drive letter without any path is not allowed"_s };

    auto firstChar = pathString[0];
    bool isValidChar = (firstChar > U'a' && firstChar < U'z') || (firstChar > U'A' && firstChar < U'Z');

    if (!isValidChar)
      return std::unexpected { U"Illegal drive letter"_s };

    result.m_driveLetter = static_cast<char>(firstChar);
    index = 2;
  }

  PathSeparator previousPathSep = PathSeparator::NONE;

  if (isPathSeparator(pathString[index])) {
    previousPathSep = pathString[index] == '/' ? PathSeparator::SLASH : PathSeparator::BACKSLASH;
    ++index;
  }

  while (index < pathString.GetLength()) {
    if (isPathSeparator(pathString[index]))
      return std::unexpected { U"Two path separators in a row"_s };

    size_t partEnd = index+1;
    while (partEnd < pathString.GetLength() && !isPathSeparator(pathString[partEnd]))
      ++partEnd;

    result.m_parts.push_back(PathPart {
      .leadingSeparator = previousPathSep,
      .text = pathString.Slice(index, partEnd-index)
    });

    if (partEnd >= pathString.GetLength()) {
      // Reached the end
      return result;
    }

    previousPathSep = pathString[partEnd] == '/' ? PathSeparator::SLASH : PathSeparator::BACKSLASH;
    index = partEnd + 1;
  }

  result.m_trailingSeparator = previousPathSep;
  return result;
}

String Path::ToString() const {
  String result;

  if (m_driveLetter != 0) {
    result += static_cast<char32_t>(m_driveLetter);
    result += U':';
  }

  auto appendSeparator = [&](PathSeparator separator) {
    if (separator == PathSeparator::NONE)
      return;

    result += (separator == PathSeparator::SLASH ? U'/' : U'\\');
  };

  for (auto const& [leadingSeparator, text] : m_parts) {
    appendSeparator(leadingSeparator);
    result += text;
  }

  appendSeparator(m_trailingSeparator);

  return result;
}