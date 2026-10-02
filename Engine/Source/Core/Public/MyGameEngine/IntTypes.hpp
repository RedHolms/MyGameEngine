#pragma once

#include <MyGameEngine/Platform/PlatformInfo.hpp>

/// Generic unsigned integer type
using uint = unsigned int;

/// @name Sized integer types
/// @{
using int8 = char;
using int16 = short;
using int32 = long;
using int64 = long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned long;
using uint64 = unsigned long long;
/// @}

/// @name Machine word type
/// @{
#if IS_32BIT
using umword_t = unsigned int;
using smword_t = signed int;
#elif IS_64BIT
using umword_t = unsigned long long;
using smword_t = signed long long;
#endif
/// @}

/// Unsigned integer type capable of storing any possible memory address
using memaddr_t = umword_t;
static_assert(sizeof(memaddr_t) >= sizeof(void*));

/// Signed integer type capable of storing any possible memory address
using smemaddr_t = smword_t;
static_assert(sizeof(smemaddr_t) >= sizeof(void*));

/// Signed integer type capable of storing size of any object placed in memory
using ssize_t = smword_t;

/// Unsigned integer type that can be used as an index in any object placed in memory
using index_t = memaddr_t;

/// Signed integer type that can be used as an index in any object placed in memory. Negative indices handled by each
/// container type individually, but it's recommended that negative values produces an index by expression:
/// <code>(Size + I)</code>, where @c Size - size of the indexed object and @c I - the negative index.
using sindex_t = smemaddr_t;
