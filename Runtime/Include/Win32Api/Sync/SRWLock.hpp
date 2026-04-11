#pragma once

namespace Win32::SRWLock {

// Win32 name: AcquireSRWLockExclusive
void AcquireExclusive(void** srwLockPtr);

// Win32 name: AcquireSRWLockShared
void AcquireShared(void** srwLockPtr);

// Win32 name: ReleaseSRWLockExclusive
void ReleaseExclusive(void** srwLockPtr);

// Win32 name: ReleaseSRWLockShared
void ReleaseShared(void** srwLockPtr);

// Win32 name: TryAcquireSRWLockExclusive
bool TryAcquireExclusive(void** srwLockPtr);

// Win32 name: TryAcquireSRWLockShared
bool TryAcquireShared(void** srwLockPtr);

} // namespace Win32::SRWLock
