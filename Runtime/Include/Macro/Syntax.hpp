#pragma once

// Mark branches that are unlikely to be executed
#define UNLIKELY [[unlikely]]

// Mark branches that are likely to be executed
#define LIKELY [[likely]]

// Force function to be inlined. Note that in some scenarios function can be generated anyway
#define forceinline __forceinline

// Explicitly mark constructor or operator implicit
#define implicit explicit(false)

// Get constexpr-friendly null terminated string containing type name
#define type_name(TYPE) (__typeinfo<TYPE>.name)

// Get 'TypeInfo const&' representing 'TYPE'
#define typeid(TYPE) (*reinterpret_cast<TypeInfo const*>(&::__typeinfo<TYPE>))
