#pragma once

#include "Utils/PImpl.hpp"

class Mutex {
  friend class CondVar;

public:
  Mutex();
  ~Mutex();

  Mutex(Mutex const&) noexcept = delete;
  Mutex(Mutex&&) noexcept = delete;
  Mutex& operator=(Mutex const&) noexcept = delete;
  Mutex& operator=(Mutex&&) noexcept = delete;

public:
  void Lock() noexcept;
  void Unlock() noexcept;
  bool TryLock() noexcept;

private:
  DECLARE_PIMPL(sizeof(void*));
};
