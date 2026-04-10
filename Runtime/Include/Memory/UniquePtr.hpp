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

public:
  constexpr T* Get() const noexcept {
    return m_pointer;
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
