#pragma once

long _InterlockedIncrement(long volatile*);
long _InterlockedDecrement(long volatile*);
#pragma intrinsic(_InterlockedIncrement, _InterlockedDecrement)

class AtomicCounter {
public:
  implicit consteval AtomicCounter(long initial) noexcept : m_count(initial) {}

public:
  forceinline long Increment() noexcept {
    return _InterlockedIncrement(&m_count);
  }

  forceinline long Decrement() noexcept {
    return _InterlockedDecrement(&m_count);
  }

  forceinline long GetCount() const noexcept {
    return m_count;
  }

private:
  long m_count;
};
