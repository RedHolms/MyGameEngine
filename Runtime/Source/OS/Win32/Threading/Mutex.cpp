#include "Threading/Mutex.hpp"

#include "Mutex.Data.hpp"
#include "Win32Api/Sync/SRWLock.hpp"

Mutex::Mutex() = default;
Mutex::~Mutex() = default;

void Mutex::Lock() noexcept {
  Win32::SRWLock::AcquireExclusive(&m->srwLock);
}

void Mutex::Unlock() noexcept {
  Win32::SRWLock::ReleaseExclusive(&m->srwLock);
}

bool Mutex::TryLock() noexcept {
  return Win32::SRWLock::TryAcquireExclusive(&m->srwLock);
}
