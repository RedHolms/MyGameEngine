#pragma once

#include "Array.hpp"

using Destructor = void(*)(void*);

union TypeFlags {
  struct {
    bool isIntegral : 1;
    bool _padding_1 : 1;
    bool _padding_2 : 1;
    bool _padding_3 : 1;
    bool _padding_4 : 1;
    bool _padding_5 : 1;
    bool _padding_6 : 1;
    bool _padding_7 : 1;

    bool isBitwiseRelocatable : 1;
  };
  uint64_t packed {};
};

namespace Detail::_Typename {

consteval size_t _StringLength(const char* string) {
  size_t length = 0;
  while (string[length])
    ++length;
  return length;
}

template <typename T>
consteval const char* _GetTemplatedFunctionSignature() {
  return __FUNCSIG__;
}

struct _TemplatedFunctionSignatureTypeSkip_t {
  size_t leftSkip;
  size_t rightSkip;
};

consteval auto _Find_TemplatedFunctionSignatureTypeSkip() {
  auto const sig = _GetTemplatedFunctionSignature<wchar_t>();
  auto const length = _StringLength(sig);

  _TemplatedFunctionSignatureTypeSkip_t skip {
    .leftSkip = (size_t)-1,
    .rightSkip = (size_t)-1
  };

  for (size_t i = 0; i+6 < length; ++i) {
    if (sig[i] == 'w' &&
      sig[i+1] == 'c' &&
      sig[i+2] == 'h' &&
      sig[i+3] == 'a' &&
      sig[i+4] == 'r' &&
      sig[i+5] == '_' &&
      sig[i+6] == 't'
    ) {
      skip.leftSkip = i;
      skip.rightSkip = length - (i+7);
      break;
    }
  }

  return skip;
}

constexpr auto _TemplatedFunctionSignatureTypeSkip = _Find_TemplatedFunctionSignatureTypeSkip();
static_assert(_TemplatedFunctionSignatureTypeSkip.leftSkip != (size_t)-1);
static_assert(_TemplatedFunctionSignatureTypeSkip.rightSkip != (size_t)-1);

struct _ConstString {
  const char* string;
  size_t length;
};

template <typename T>
consteval _ConstString _Get() {
  auto const sig = _GetTemplatedFunctionSignature<T>();
  auto const length = _StringLength(sig);

  auto const& skip = _TemplatedFunctionSignatureTypeSkip;
  return _ConstString {
    .string = sig + skip.leftSkip,
    .length = (length - skip.rightSkip) - skip.leftSkip
  };
}

} // namespace Details::_Typename

// Produces destructor function for 'T'
template <class T>
inline void __destructor(void* object) {
  static_cast<T*>(object)->~T();
}

template <size_t N>
struct __TypeInfo {
  size_t size {};
  size_t alignment {};
  Destructor destructor {};
  TypeFlags flags {};
  char name[N] {};
};

/**
 * Runtime type information
 */
class TypeInfo {
public:
  TypeInfo() = delete;
  ~TypeInfo() = delete;

public:
  template <typename T>
  static consteval auto _CreateTypeInfo() noexcept {
    constexpr auto typeName = Detail::_Typename::_Get<T>();

    __TypeInfo<typeName.length+1> typeInfo {};

    typeInfo.size = sizeof(T);
    typeInfo.alignment = alignof(T);
    typeInfo.destructor = &__destructor<T>;
    typeInfo.flags.packed = 0;

    for (size_t i = 0; i < typeName.length; ++i)
      typeInfo.name[i] = typeName.string[i];
    typeInfo.name[typeName.length] = 0;

    return typeInfo;
  }

public:
  constexpr size_t GetSize() const noexcept {
    return m_size;
  }

  constexpr size_t GetAlignment() const noexcept {
    return m_alignment;
  }

  constexpr Destructor GetDestructor() const noexcept {
    return m_destructor;
  }

  constexpr TypeFlags GetFlags() const noexcept {
    return m_flags;
  }

  constexpr const char* GetName() const noexcept {
    return m_name;
  }

public:
  size_t m_size;
  size_t m_alignment;
  Destructor m_destructor;
  TypeFlags m_flags;
  char m_name[];
};

template <typename T>
inline constinit auto __typeinfo = TypeInfo::_CreateTypeInfo<T>();
