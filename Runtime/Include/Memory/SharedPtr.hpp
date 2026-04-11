#pragma once

#include "Threading/AtomicCounter.hpp"

template <typename T>
class SharedPtr {
public:
  constexpr SharedPtr() noexcept = default;

  explicit SharedPtr(T* pointer) noexcept
    : m_pointer(pointer),
      m_refCount(new RefCount{}) {}

  ~SharedPtr() {
    _DecRef();
  }

  SharedPtr(SharedPtr const& other) noexcept : m_pointer(other.m_pointer), m_refCount(other.m_refCount) {
    _IncRef();
  }

  SharedPtr& operator=(SharedPtr const& other) noexcept {
    Reset();
    m_pointer = other.m_pointer;
    m_refCount = other.m_refCount;
    _IncRef();
    return *this;
  }

  SharedPtr(SharedPtr&& other) noexcept : m_pointer(other.m_pointer), m_refCount(other.m_refCount) {
    other.m_pointer = nullptr;
    other.m_refCount = nullptr;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    Reset();
    m_pointer = other.m_pointer;
    m_refCount = other.m_refCount;
    other.m_pointer = nullptr;
    other.m_refCount = nullptr;
    return *this;
  }

public:
  constexpr T* Get() const noexcept {
    return m_pointer;
  }

  void Reset() {
    if (_DecRef()) {
      m_pointer = nullptr;
      m_refCount = nullptr;
    }
  }

  constexpr T* operator->() const noexcept {
    return m_pointer;
  }

private:
  void _IncRef() {
    if (m_refCount)
      m_refCount->refs.Increment();
  }

  bool _DecRef() {
    if (!m_refCount)
      return false;

    auto remain = m_refCount->refs.Decrement();
    if (remain == 0) {
      // delete the object
      delete m_pointer;
      delete m_refCount;
      return true;
    }

    return false;
  }

private:
  struct RefCount {
    AtomicCounter refs = 1;
  };

  T* m_pointer = nullptr;
  RefCount* m_refCount = nullptr;
};

template <typename T, typename... ArgsT>
constexpr SharedPtr<T> MakeShared(ArgsT&&... args) noexcept {
  return SharedPtr { new T(ForwardArg<ArgsT>(args)...) };
}
