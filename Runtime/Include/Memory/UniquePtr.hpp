#pragma once

template <typename T>
class UniquePtr {
public:
  consteval UniquePtr() = default;

  explicit constexpr UniquePtr(T* pointer) noexcept : m_pointer(pointer) {}

  constexpr ~UniquePtr() noexcept {
    if (m_pointer)
      delete m_pointer;
  }

  constexpr UniquePtr(UniquePtr&& other) noexcept : m_pointer(other.Release()) {}

  constexpr UniquePtr& operator=(UniquePtr&& other) noexcept {
    Reset();
    m_pointer = other.Release();
    return *this;
  }

  UniquePtr(UniquePtr const&) = delete;
  UniquePtr& operator=(UniquePtr const&) = delete;

public:
  constexpr T* Get() const noexcept {
    return m_pointer;
  }

  constexpr T* Release(T* newValue = nullptr) noexcept {
    auto result = m_pointer;
    m_pointer = newValue;
    return result;
  }

  constexpr void Reset() noexcept {
    if (m_pointer)
      delete m_pointer;
    m_pointer = nullptr;
  }

  constexpr T* operator->() const noexcept {
    return m_pointer;
  }

private:
  T* m_pointer = nullptr;
};

template <typename T, typename... ArgsT>
constexpr UniquePtr<T> MakeUnique(ArgsT&&... args) noexcept {
  return UniquePtr { new T(ForwardArg<ArgsT>(args)...) };
}
