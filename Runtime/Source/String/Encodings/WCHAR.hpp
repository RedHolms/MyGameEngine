#pragma once

#include "Base.hpp"
#include "UTF16.hpp"

template <>
struct EncodingFunctions<Encoding::WCHAR> : EncodingFunctions<Encoding::UTF16> {};
