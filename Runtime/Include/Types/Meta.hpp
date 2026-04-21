#pragma once

template <typename T>
struct TypeIdentity {
  using Type = T;
};

template <typename T>
using ExplicitType = TypeIdentity<T>::Type;
