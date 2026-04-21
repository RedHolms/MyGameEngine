#pragma once

// Operating systems enumeration
#define OS_WINDOWS   0
#define OS_GNU_LINUX 1
#define OS_MACOS     2

// ABI Families enumeration
#define ABI_FAMILY_WINDOWS 0
#define ABI_FAMILY_SYSV    1

// Architectures enumeration
#define ARCH_AMD64     0
#define ARCH_I386      1
#define ARCH_ARM       2
#define ARCH_ARM_THUMB 3
#define ARCH_AARCH64   4

// Construct ABI identifier from family and arch
#define MAKE_ABI_ENUM(FAMILY, ARCH) ((ABI_FAMILY_##FAMILY << 8) | ARCH_##ARCH)

// ABI enumeration (family + arch)
#define ABI_WINDOWS_AMD64     MAKE_ABI_ENUM(WINDOWS, AMD64)
#define ABI_WINDOWS_I386      MAKE_ABI_ENUM(WINDOWS, I386)
#define ABI_WINDOWS_ARM       MAKE_ABI_ENUM(WINDOWS, ARM)
#define ABI_WINDOWS_ARM_THUMB MAKE_ABI_ENUM(WINDOWS, ARM_THUMB)
#define ABI_WINDOWS_AARCH64   MAKE_ABI_ENUM(WINDOWS, AARCH64)
#define ABI_SYSV_AMD64        MAKE_ABI_ENUM(SYSV, AMD64)
#define ABI_SYSV_I386         MAKE_ABI_ENUM(SYSV, I386)
#define ABI_SYSV_ARM          MAKE_ABI_ENUM(SYSV, ARM)
#define ABI_SYSV_ARM_THUMB    MAKE_ABI_ENUM(SYSV, ARM_THUMB)
#define ABI_SYSV_AARCH64      MAKE_ABI_ENUM(SYSV, AARCH64)

// Determine current OS
#if defined(_WIN32)
#define CURRENT_OS OS_WINDOWS
#elif defined(__gnu_linux__)
#define CURRENT_OS OS_GNU_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define CURRENT_OS OS_MACOS
#else
#error "Couldn't determine your OS"
#endif

#define IS_OS(TARGET)     (CURRENT_OS == MACRO_RESOLVE(OS_##TARGET))

#define IS_WINDOWS_OS     IS_OS(WINDOWS)
#define IS_GNU_LINUX_OS   IS_OS(GNU_LINUX)
#define IS_MACOS_OS       IS_OS(MACOS)

#define IS_LINUX_BASED_OS (IS_GNU_LINUX_OS || IS_MACOS_OS)

#if IS_WINDOWS_OS
#define CURRENT_OS_NAME     "Windows"
#define CURRENT_OS_CODENAME WINDOWS
#elif IS_GNU_LINUX_OS
#define CURRENT_OS_NAME     "GNU/Linux"
#define CURRENT_OS_CODENAME GNU_LINUX
#elif IS_MACOS_OS
#define CURRENT_OS_NAME     "MacOS"
#define CURRENT_OS_CODENAME MACOS
#endif

// Determine current ABI family
#if defined(_WIN32)
#define CURRENT_ABI_FAMILY ABI_FAMILY_WINDOWS
#else
// Assume SysV on any non-Windows system
#define CURRENT_ABI_FAMILY ABI_FAMILY_SYSV
#endif

#define IS_ABI_FAMILY(TARGET) (CURRENT_ABI_FAMILY == MACRO_RESOLVE(ABI_FAMILY_##TARGET))

#define IS_WINDOWS_ABI_FAMILY IS_ABI_FAMILY(WINDOWS)
#define IS_SYSV_ABI_FAMILY    IS_ABI_FAMILY(SYSV)

#if IS_WINDOWS_ABI_FAMILY
#define CURRENT_ABI_FAMILY_NAME     "Windows"
#define CURRENT_ABI_FAMILY_CODENAME WINDOWS
#elif IS_SYSV_ABI_FAMILY
#define CURRENT_ABI_FAMILY_NAME     "SysV"
#define CURRENT_ABI_FAMILY_CODENAME SYSV
#endif

// Determine current arch
#if defined(__amd64__) || defined(__amd64) || defined(_M_X64) || defined(_M_AMD64)
#define CURRENT_ARCH ARCH_AMD64
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) ||               \
  defined(__i686__) || defined(__IA32__) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) ||                  \
  defined(__I86__) || defined(__386)
#define CURRENT_ARCH ARCH_I386
#elif defined(__aarch64__) || defined(_M_ARM64)
#define CURRENT_ARCH ARCH_AARCH64
#elif defined(__thumb__) || defined(_M_ARMT)
#define CURRENT_ARCH ARCH_ARM_THUMB
#elif defined(__arm__) || defined(_M_ARMT)
#define CURRENT_ARCH ARCH_ARM
#else
#error "Couldn't determine your architecture"
#endif

#define IS_ARCH(TARGET)   (CURRENT_ARCH == MACRO_RESOLVE(ARCH_##TARGET))

#define IS_AMD64_ARCH     IS_ARCH(AMD64)
#define IS_I386_ARCH      IS_ARCH(I386)
#define IS_ARM_ARCH       IS_ARCH(ARM)
#define IS_ARM_THUMB_ARCH IS_ARCH(ARM_THUMB)
#define IS_AARCH64_ARCH   IS_ARCH(AARCH64)

#define IS_32_BIT         (IS_I386_ARCH || IS_ARM_ARCH || IS_ARM_THUMB_ARCH)
#define IS_64_BIT         (IS_AMD64_ARCH || IS_AARCH64_ARCH)

#if IS_AMD64_ARCH
#define CURRENT_ARCH_NAME     "AMD64"
#define CURRENT_ARCH_CODENAME AMD64
#elif IS_I386_ARCH
#define CURRENT_ARCH_NAME     "i386"
#define CURRENT_ARCH_CODENAME I386
#endif

// Make ABI definition
#define __MAKE_CURRENT_ABI_2(FAMILY, ARCH)   ABI_##FAMILY##_##ARCH
#define __MAKE_CURRENT_ABI_1(FAMILY, ARCH)   __MAKE_CURRENT_ABI_2(FAMILY, ARCH)
#define CURRENT_ABI                          __MAKE_CURRENT_ABI_1(CURRENT_ABI_FAMILY_CODENAME, CURRENT_ARCH_CODENAME)

#define IS_ABI(TARGET)                       (CURRENT_ABI == MACRO_RESOLVE(ABI_##TARGET))

#define IS_WINDOWS_AMD64_ABI                 IS_ABI(WINDOWS_AMD64)
#define IS_WINDOWS_I386_ABI                  IS_ABI(WINDOWS_I386)
#define IS_WINDOWS_ARM_ABI                   IS_ABI(WINDOWS_ARM)
#define IS_WINDOWS_ARM_THUMB_ABI             IS_ABI(WINDOWS_ARM_THUMB)
#define IS_WINDOWS_AARCH64_ABI               IS_ABI(WINDOWS_AARCH64)
#define IS_SYSV_AMD64_ABI                    IS_ABI(SYSV_AMD64)
#define IS_SYSV_I386_ABI                     IS_ABI(SYSV_I386)
#define IS_SYSV_ARM_ABI                      IS_ABI(SYSV_ARM)
#define IS_SYSV_ARM_THUMB_ABI                IS_ABI(SYSV_ARM_THUMB)
#define IS_SYSV_AARCH64_ABI                  IS_ABI(SYSV_AARCH64)
