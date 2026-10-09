#pragma once

#include <utility>

template <class Callable>
struct CallOnExit {
  Callable target;

  explicit constexpr CallOnExit(Callable&& target) : target(std::move(target)) {}
  constexpr ~CallOnExit() { target(); }
};
