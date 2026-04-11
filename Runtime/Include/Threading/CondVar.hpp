#pragma once

#include "Mutex.hpp"
#include "UniqueLock.hpp"
#include "Utils/PImpl.hpp"

class CondVar {
public:
  CondVar();
  ~CondVar();

  CondVar(CondVar const&) noexcept = delete;
  CondVar(CondVar&&) noexcept = delete;
  CondVar& operator=(CondVar const&) noexcept = delete;
  CondVar& operator=(CondVar&&) noexcept = delete;

public:
  void NotifyOne() noexcept;
  void NotifyAll() noexcept;

  void Wait(UniqueLock<>& lock) noexcept;

private:
  DECLARE_PIMPL(sizeof(void*));
};
