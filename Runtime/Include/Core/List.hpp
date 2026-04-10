#pragma once

#include "Memory/Utils.hpp"

// Growable array of T
template <typename T>
class List {
public:
  using ItemType = T;

public:
  class Iterator {
    friend List;

  private:
    constexpr Iterator(List* container, size_t index) noexcept
      : m_container(container), m_index(index) {}

  public:
    constexpr operator T*() const noexcept {
      return GetPtr();
    }

    constexpr T& operator*() const noexcept {
      return *GetPtr();
    }

    constexpr T* operator->() const noexcept {
      return GetPtr();
    }

    constexpr Iterator& operator++() noexcept {
      //TODO check boundary
      ++m_index;
      return *this;
    }

    constexpr Iterator operator++(int) noexcept {
      Iterator old = *this;
      ++(*this);
      return old;
    }

    constexpr Iterator& operator--() noexcept {
      //TODO check boundary
      --m_index;
      return *this;
    }

    constexpr Iterator operator--(int) noexcept {
      Iterator old = *this;
      --(*this);
      return old;
    }

    constexpr Iterator& operator+=(int diff) noexcept {
      //TODO check boundary
      m_index += diff;
      return *this;
    }

    constexpr Iterator& operator-=(int diff) noexcept {
      //TODO check boundary
      m_index -= diff;
      return *this;
    }

    constexpr bool operator==(Iterator const& other) const noexcept {
      return other.m_container == m_container && other.m_index == m_index;
    }

    constexpr bool operator!=(Iterator const&) const noexcept = default;

  private:
    constexpr T* GetPtr() const noexcept {
      return &(*m_container)[m_index];
    }

  private:
    List* const m_container;
    size_t m_index;
  };

  class ConstIterator {
    friend List;

  private:
    constexpr ConstIterator(List const* container, size_t index) noexcept
      : m_container(container), m_index(index) {}

  public:
    constexpr operator T const*() const noexcept {
      return GetPtr();
    }

    constexpr T const& operator*() const noexcept {
      return *GetPtr();
    }

    constexpr T const* operator->() const noexcept {
      return GetPtr();
    }

    constexpr ConstIterator& operator++() noexcept {
      //TODO check boundary
      ++m_index;
      return *this;
    }

    constexpr ConstIterator operator++(int) noexcept {
      ConstIterator old = *this;
      ++(*this);
      return old;
    }

    constexpr ConstIterator& operator--() noexcept {
      //TODO check boundary
      --m_index;
      return *this;
    }

    constexpr ConstIterator operator--(int) noexcept {
      ConstIterator old = *this;
      --(*this);
      return old;
    }

    constexpr ConstIterator& operator+=(int diff) noexcept {
      //TODO check boundary
      m_index += diff;
      return *this;
    }

    constexpr ConstIterator& operator-=(int diff) noexcept {
      //TODO check boundary
      m_index -= diff;
      return *this;
    }

    constexpr bool operator==(ConstIterator const& other) const noexcept {
      return other.m_container == m_container && other.m_index == m_index;
    }

    constexpr bool operator!=(ConstIterator const&) const noexcept = default;

  private:
    constexpr T const* GetPtr() const noexcept {
      return &(*m_container)[m_index];
    }

  private:
    List const* const m_container;
    size_t m_index;
  };

public:
  consteval List() noexcept = default;
  constexpr ~List() noexcept {
    _Reset();
  }

  constexpr List(List const& other) noexcept : List() {
    if (other.IsEmpty())
      return;

    _Reallocate(m_count = other.GetCount());

    if constexpr (IsTriviallyCopyConstructibleType<T>) {
      Memory::CopyItems<T>(m_buffer, other.m_buffer, m_count);
    }
    else {
      for (size_t i = 0; i < m_count; ++i)
        new (&m_buffer[i]) T (other.m_buffer[i]);
    }
  }

  constexpr List(List&& other) noexcept
    : m_buffer(other.m_buffer), m_count(other.m_count), m_capacity(other.m_capacity)
  {
    other.m_buffer = nullptr;
    other.m_count = 0;
    other.m_capacity = 0;
  }

  constexpr List& operator=(List const& other) noexcept {
    Clear();

    if (other.IsEmpty())
      return *this;

    _WantCapacity(m_count = other.GetCount());

    if constexpr (IsTriviallyCopyConstructibleType<T>) {
      Memory::CopyItems<T>(m_buffer, other.m_buffer, m_count);
    }
    else {
      for (size_t i = 0; i < m_count; ++i)
        new (&m_buffer[i]) T (other.m_buffer[i]);
    }

    return *this;
  }

  constexpr List& operator=(List&& other) noexcept {
    _Reset();
    m_buffer = other.m_buffer;
    m_count = other.m_count;
    m_capacity = other.m_capacity;
    other.m_buffer = nullptr;
    other.m_count = 0;
    other.m_capacity = 0;
    return *this;
  }

public:
  [[nodiscard]] constexpr size_t GetCount()    const noexcept { return m_count; }
  [[nodiscard]] constexpr bool   IsEmpty()     const noexcept { return m_count == 0; }
  [[nodiscard]] constexpr size_t GetCapacity() const noexcept { return m_capacity; }

  [[nodiscard]] constexpr Iterator begin() noexcept {
    return Iterator {this, 0};
  }

  [[nodiscard]] constexpr ConstIterator begin() const noexcept {
    return ConstIterator {this, 0};
  }

  [[nodiscard]] constexpr Iterator end() noexcept {
    return Iterator {this, m_count};
  }

  [[nodiscard]] constexpr ConstIterator end() const noexcept {
    return ConstIterator {this, m_count};
  }

  [[nodiscard]] constexpr Iterator at(index_t index) noexcept {
    return Iterator {this, _TranslateIndex(index)};
  }

  [[nodiscard]] constexpr ConstIterator at(index_t index) const noexcept {
    return ConstIterator {this, _TranslateIndex(index)};
  }

  [[nodiscard]] constexpr T& operator[](index_t index) noexcept {
    return m_buffer[_TranslateIndex(index)];
  }

  [[nodiscard]] constexpr T const& operator[](index_t index) const noexcept {
    return m_buffer[_TranslateIndex(index)];
  }

public:
  constexpr void Reserve(size_t newCapacity) noexcept {
    if (newCapacity > m_capacity)
      _Reallocate(newCapacity);
  }

  constexpr void Clear() noexcept {
    if constexpr (!IsTriviallyDestructibleType<T>) {
      for (size_t i = 0; i < m_count; ++i)
        m_buffer[i].~T();
    }

    m_count = 0;
  }

  template <typename... ArgsT>
  constexpr T& Append(ArgsT&&... args) noexcept {
    _WantAddItem();

    auto const itemPtr = &m_buffer[m_count++];
    new (itemPtr) T (ForwardArg<ArgsT>(args)...);

    return *itemPtr;
  }

  template <typename... ArgsT>
  constexpr T& Insert(index_t atIndex, ArgsT&&... args) noexcept {
    size_t index = _TranslateIndex(atIndex);

    if (index >= m_count) UNLIKELY
      return Append(ForwardArg<ArgsT>(args)...);

    _WantAddItem();

    T* insertPtr = &m_buffer[index];

    if constexpr (IsTriviallyCopyableType<T>) {
      Memory::MoveItems<T>(insertPtr + 1, insertPtr, m_count - index);
    }
    else {
      // Move backward to avoid overwrite
      for (ssize_t i = m_count; i > index; --i) {
        T* dst = &m_buffer[i];
        T* src = &m_buffer[i - 1];

        new (dst) T(Move(*src));
        src->~T();
      }
    }

    new (insertPtr) T(ForwardArg<ArgsT>(args)...);
    ++m_count;

    return *insertPtr;
  }

  constexpr void Remove(index_t atIndex, size_t count = 1) noexcept {
    if (count == 0 || m_count == 0) UNLIKELY
      return;

    size_t index = _TranslateIndex(atIndex);

    if (index >= m_count) UNLIKELY
      return;

    if (index + count > m_count)
      count = m_count - index;

    T* removePtr = &m_buffer[index];
    T* tailPtr = &m_buffer[index + count];
    size_t tailCount = m_count - (index + count);

    // Destroy removed elements
    if constexpr (!IsTriviallyDestructibleType<T>) {
      for (size_t i = 0; i < count; ++i)
        removePtr[i].~T();
    }

    // Shift tail
    if (tailCount > 0) {
      if constexpr (IsTriviallyCopyableType<T>) {
        Memory::MoveItems<T>(removePtr, tailPtr, tailCount);
      }
      else {
        for (size_t i = 0; i < tailCount; ++i) {
          T* dst = &removePtr[i];
          T* src = &tailPtr[i];

          new (dst) T(Move(*src));
          src->~T();
        }
      }
    }

    m_count -= count;
  }

private:
  constexpr size_t _TranslateIndex(index_t index) const noexcept {
    if (index < 0)
      return m_count + index;
    return index;
  }

  constexpr void _WantAddItem() noexcept {
    _WantCapacity(m_count + 1);
  }

  constexpr void _WantCapacity(size_t required) noexcept {
    if (m_capacity < required) UNLIKELY {
      size_t newCapacity = m_capacity * 2;
      if (newCapacity < required)
        newCapacity = required;
      _Reallocate(newCapacity);
    }
  }

  constexpr void _Reset() {
    if constexpr (!IsTriviallyDestructibleType<T>) {
      for (size_t i = 0; i < m_count; ++i)
        m_buffer[i].~T();
    }

    delete[] m_buffer;
  }

  constexpr void _Reallocate(size_t newCapacity) noexcept {
    // ASSERT: newCapacity >= m_count

    auto const newBuffer = new T[newCapacity];
    m_capacity = newCapacity;

    if (m_buffer == nullptr) UNLIKELY {
      m_buffer = newBuffer;
      return;
    }

    if (m_count > 0) LIKELY {
      if constexpr (IsTriviallyCopyableType<T>) {
        Memory::CopyItems<T>(newBuffer, m_buffer, m_count);
      }
      else {
        for (size_t i = 0; i < m_count; ++i) {
          auto& source = m_buffer[i];
          new (&newBuffer[i]) T (Move(source));
          source.~T();
        }
      }
    }

    delete[] m_buffer;
    m_buffer = newBuffer;
  }

private:
  T* m_buffer = nullptr;
  size_t m_count = 0; // Count of constructed elements in buffer
  size_t m_capacity = 0; // Max count of elements in buffer
};
