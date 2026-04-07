#pragma once

// TODO maybe move it somewhere?
[[nodiscard]] constexpr bool IsConstantEvaluated() noexcept {
  return __builtin_is_constant_evaluated();
}

#include "Marcoses/Helpers.hpp"
#include "Marcoses/Platform.hpp"
#include "Marcoses/Syntax.hpp"
#include "Memory/Allocators.hpp"
#include "Types/Meta.hpp"
#include "Types/Numerics.hpp"
