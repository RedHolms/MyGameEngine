#include "Win32Api/Sync/SRWLock.hpp"

#include "OS/Win32/Api/DeclSpecs.hpp"

extern "C" {

WIN32_FUNC void WIN32_API AcquireSRWLockExclusive(void**);
WIN32_FUNC void WIN32_API AcquireSRWLockShared(void**);
WIN32_FUNC void WIN32_API ReleaseSRWLockExclusive(void**);
WIN32_FUNC void WIN32_API ReleaseSRWLockShared(void**);
WIN32_FUNC uint8_t WIN32_API TryAcquireSRWLockExclusive(void**);
WIN32_FUNC uint8_t WIN32_API TryAcquireSRWLockShared(void**);

} // extern "C"

void Win32::SRWLock::AcquireExclusive(void** srwLockPtr) {
  ::AcquireSRWLockExclusive(srwLockPtr);
}

void Win32::SRWLock::AcquireShared(void** srwLockPtr) {
  ::AcquireSRWLockShared(srwLockPtr);
}

void Win32::SRWLock::ReleaseExclusive(void** srwLockPtr) {
  ::ReleaseSRWLockExclusive(srwLockPtr);
}

void Win32::SRWLock::ReleaseShared(void** srwLockPtr) {
  ::ReleaseSRWLockShared(srwLockPtr);
}

bool Win32::SRWLock::TryAcquireExclusive(void** srwLockPtr) {
  return !!::TryAcquireSRWLockExclusive(srwLockPtr);
}

bool Win32::SRWLock::TryAcquireShared(void** srwLockPtr) {
  return !!::TryAcquireSRWLockShared(srwLockPtr);
}

