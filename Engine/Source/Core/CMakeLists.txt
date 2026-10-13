MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
)

target_sources(${TARGET} PRIVATE
  "Private/String/Encodings/ASCII.cpp"
  "Private/String/Encodings/ASCII.hpp"
  "Private/String/Encodings/Base.hpp"
  "Private/String/Encodings/Encodings.cpp"
  "Private/String/Encodings/UTF8.cpp"
  "Private/String/Encodings/UTF8.hpp"
  "Private/String/Encodings/UTF16.cpp"
  "Private/String/Encodings/UTF16.hpp"
  "Private/String/Encodings/UTF32.cpp"
  "Private/String/Encodings/UTF32.hpp"
  "Private/String/Encodings/WCHAR.hpp"
  "Private/Assert.cpp"
  "Private/Console.cpp"
  "Private/Logging.cpp"
  "Private/ThreadInfo.cpp"
)

target_sources(${TARGET} PUBLIC
  "Public/MyGameEngine/Logging/VerbosityColors.hpp"
  "Public/MyGameEngine/Platform/PlatformInfo.hpp"
  "Public/MyGameEngine/Assert.hpp"
  "Public/MyGameEngine/Bytes.hpp"
  "Public/MyGameEngine/Console.hpp"
  "Public/MyGameEngine/EncodedString.hpp"
  "Public/MyGameEngine/Guards.hpp"
  "Public/MyGameEngine/IntTypes.hpp"
  "Public/MyGameEngine/Logging.hpp"
  "Public/MyGameEngine/MemoryUtils.hpp"
  "Public/MyGameEngine/String.hpp"
  "Public/MyGameEngine/ThreadInfo.hpp"
  "Public/MyGameEngine/Win32Inc.hpp"
)

target_link_libraries(${TARGET} PUBLIC fmt::fmt)
