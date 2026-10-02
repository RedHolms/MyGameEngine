#pragma once

// Right now we support only Windows/AMD64

/// @name Supported operating systems enumeration
/// @{
#define OS_WINDOWS 0
/// @}

/// @name Supported ABI families enumeration
/// @{
#define ABI_FAMILY_WINDOWS 0
/// @}

/// @name Supported architectures enumeration
/// @{
#define ARCH_AMD64 0
/// @}

/// @name Target OS determination
/// @{
#if defined(_WIN32)
#define TARGET_OS          OS_WINDOWS
#define TARGET_OS_NAME     "Windows"
#define TARGET_OS_CODENAME WINDOWS
#else
#error "Target OS is not supported"
#endif
/// @}

/// @name Target ABI family determination
/// @{
#if defined(_WIN32)
#define TARGET_ABI_FAMILY          ABI_FAMILY_WINDOWS
#define TARGET_ABI_FAMILY_NAME     "Windows"
#define TARGET_ABI_FAMILY_CODENAME WINDOWS
#else
#error "Target ABI family is not supported"
#endif
/// @}

/// @name Target architecture determination
/// @{
#if defined(__amd64__) || defined(__amd64) || defined(_M_X64) || defined(_M_AMD64)
#define TARGET_ARCH          ARCH_AMD64
#define TARGET_ARCH_NAME     "AMD64"
#define TARGET_ARCH_CODENAME AMD64
#else
#error "Target architecture is not supported"
#endif
/// @}

/// @name Helpers
/// @{
#define IS_TARGET_OS_WINDOWS (TARGET_OS == OS_WINDOWS)

#define IS_TARGET_ABI_FAMILY_WINDOWS (TARGET_ABI_FAMILY == ABI_FAMILY_WINDOWS)

#define IS_TARGET_ARCH_AMD64 (TARGET_ARCH == ARCH_AMD64)

// Results to "true" if target architecture's machine word is 32 bits wide
#define IS_32BIT (0)

// Results to "true" if target architecture's machine word is 64 bits wide
#define IS_64BIT (IS_TARGET_ARCH_AMD64)
/// @}
