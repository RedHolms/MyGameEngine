#pragma once

#include "TLockable.hpp"

class Mutex;

template <TLockable Lockable = Mutex>
class UniqueLock {
  friend class CondVar;

public:
  explicit constexpr UniqueLock(Lockable& target) : m_target(&target), m_locked(true) {
    m_target->Lock();
  }

  constexpr ~UniqueLock() {
    _Reset();
  }

  UniqueLock(UniqueLock const&) = delete;
  UniqueLock& operator=(UniqueLock const&) = delete;

  constexpr UniqueLock(UniqueLock&& other) noexcept : m_target(other.m_target), m_locked(other.m_locked) {
    other.m_target = nullptr;
    other.m_locked = false;
  }

  constexpr UniqueLock& operator=(UniqueLock&& other) noexcept {
    _Reset();
    m_target = other.m_target;
    other.m_target = nullptr;
    other.m_locked = false;
    return *this;
  }

public:
  constexpr void Lock() noexcept {
    if (m_locked)
      return;
    m_locked = true;
    m_target->Lock();
  }

  constexpr void Unlock() noexcept {
    if (!m_locked)
      return;
    m_locked = false;
    m_target->Unlock();
  }

private:
  constexpr void _Reset() {
    if (m_target && m_locked)
      m_target->Unlock();
  }

private:
  Lockable* m_target;
  bool m_locked;
};
