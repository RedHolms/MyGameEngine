#pragma once

#include <Private/String/Encodings/Base.hpp>
#include <Private/String/Encodings/UTF16.hpp>

template <>
struct EncodingFunctions<Encoding::WCHAR> : EncodingFunctions<Encoding::UTF16> {};
