#include "Threading/CondVar.hpp"

#include "Mutex.Data.hpp"
#include "Win32Api/Sync/CondVar.hpp"

struct CondVar::Data {
  void* condVar = nullptr;
};

CondVar::CondVar() = default;
CondVar::~CondVar() = default;

void CondVar::NotifyOne() noexcept {
  Win32::CondVar::WakeOne(&m->condVar);
}

void CondVar::NotifyAll() noexcept {
  Win32::CondVar::WakeAll(&m->condVar);
}

void CondVar::Wait(UniqueLock<>& lock) noexcept {
  if (!lock.m_target || !lock.m_locked)
    return;

  auto srwLockPtr = &lock.m_target->m->srwLock;
  Win32::CondVar::SleepSRW(&m->condVar, srwLockPtr, 0xFFFFFFFF, 0);
}

