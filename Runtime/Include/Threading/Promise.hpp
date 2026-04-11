#pragma once

#include "Memory/SharedPtr.hpp"
#include "Threading/CondVar.hpp"
#include "Threading/Mutex.hpp"

template <typename T>
struct PromiseFuturePair;

template <typename T>
inline PromiseFuturePair<T> MakePromise();

template <typename T>
class _AsyncResultState {
public:
  void Wait() {
    UniqueLock _lock(m_mutex);
    while (!m_resolved)
      m_condVar.Wait(_lock);
  }

  T& Get() noexcept {
    Wait();
    return m_result;
  }

  T const& Get() const noexcept {
    Wait();
    return m_result;
  }

  void Resolve(T const& value) {
    {
      UniqueLock _lock(m_mutex);
      if (m_resolved)
        return;

      m_resolved = true;
      new (&m_result) T (value);
    }

    m_condVar.NotifyAll();
  }

  void Resolve(T&& value) {
    {
      UniqueLock _lock(m_mutex);
      if (m_resolved)
        return;

      m_resolved = true;
      new (&m_result) T (ForwardArg<T>(value));
    }

    m_condVar.NotifyAll();
  }

private:
  union {
    char _dummy[sizeof(T)] {};
    T m_result;
  };

  Mutex m_mutex;
  CondVar m_condVar;
  bool m_resolved = false;
};

template <typename T>
class Future {
  friend PromiseFuturePair<T> MakePromise();

private:
  explicit Future(SharedPtr<_AsyncResultState<T>> const& state) : m_state(state) {}

public:
  inline void Wait() {
    m_state->Wait();
  }

  inline T& Get() noexcept {
    return m_state->Get();
  }

  inline T const& Get() const noexcept {
    return m_state->Get();
  }

private:
  SharedPtr<_AsyncResultState<T>> m_state;
};

template <typename T>
class Promise {
  friend PromiseFuturePair<T> MakePromise();

private:
  explicit Promise(SharedPtr<_AsyncResultState<T>> const& state) : m_state(state) {}

public:
  inline void Resolve(T const& value) {
    m_state->Resolve(value);
  }

  inline void Resolve(T&& value) {
    m_state->Resolve(ForwardArg<T>(value));
  }

  // TODO: Rejection logic

private:
  SharedPtr<_AsyncResultState<T>> m_state;
};

template <typename T>
struct PromiseFuturePair {
  Promise<T> promise;
  Future<T> future;
};

template <typename T>
inline PromiseFuturePair<T> MakePromise() {
  auto sharedState = MakeShared<_AsyncResultState<T>>();
  return { Promise { sharedState }, Future { sharedState } };
}
