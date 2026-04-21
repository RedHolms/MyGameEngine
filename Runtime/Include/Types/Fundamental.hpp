#pragma once

using uint     = unsigned int;

using uint8_t  = unsigned char;
using int8_t   = signed char;
using uint16_t = unsigned short;
using int16_t  = signed short;
using uint32_t = unsigned long;
using int32_t  = signed long;
using uint64_t = unsigned long long;
using int64_t  = signed long long;

#if IS_32_BIT
using uintptr_t = unsigned int;
using intptr_t = int;
#elif IS_64_BIT
using uintptr_t = unsigned long long;
using intptr_t = long long;
#endif

static_assert(sizeof(uintptr_t) == sizeof(void*));
static_assert(sizeof(intptr_t) == sizeof(void*));

using size_t = uintptr_t;
using ssize_t = intptr_t;

using index_t = ssize_t;

using ptrdiff_t = decltype((int*)2 - (int*)1);
using nullptr_t = decltype(nullptr);
