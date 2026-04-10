#pragma once

using uint = unsigned int;

using uint8_t   = unsigned char;
using int8_t    = signed char;
using uint16_t  = unsigned short;
using int16_t   = signed short;
using uint32_t  = unsigned long;
using int32_t   = signed long;
using uint64_t  = unsigned long long;
using int64_t   = signed long long;

template <uint VoidPointerSize>
struct __DetermineMachineWordType;

template <>
struct __DetermineMachineWordType<4> {
  using _Unsigned = unsigned long;
  using _Signed = long;
};

template <>
struct __DetermineMachineWordType<8> {
  using _Unsigned = unsigned long long;
  using _Signed = long long;
};

using __MachineWord = __DetermineMachineWordType<sizeof(void*)>;
using machine_word_t = __MachineWord::_Unsigned;
using signed_machine_word_t = __MachineWord::_Signed;

using size_t = machine_word_t;
using ssize_t = signed_machine_word_t;

// In our architecture almost all containers accept negative indices
//  (-1 is the last element, -2 is the element before the last, etc.)
using index_t = signed_machine_word_t;

// Numeric types that can hold void*
using uintptr_t = machine_word_t;
using intptr_t = signed_machine_word_t;

