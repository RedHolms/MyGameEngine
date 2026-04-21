#pragma once

/**
 * Order MATTERS!!!
 */

#include "Macro/Utilities.hpp"
#include "Macro/Platform.hpp"
#include "Macro/Syntax.hpp"
#include "Types/Fundamental.hpp"

// 'String' is tricky because it's implementation throws exceptions and 'Exception' class uses 'String' for info.
// Sad and ugly, but we're forced to split impl in 2 parts
#include "__string_decl.hpp"

#include "CommonUtilities.hpp"

#include "Types/Casts.hpp"
#include "Types/Meta.hpp"
#include "Types/Traits.hpp"
#include "Types/TypeInfo.hpp"

#include "Exceptions.hpp"

#include "Memory/Allocators.hpp"
#include "Memory/Utils.hpp"

#include "__string_impl.hpp"

#include "Containers.hpp"
#include "Array.hpp"

#include "__string_impl2.hpp"

#include "Format.hpp"
