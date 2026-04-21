#pragma once

class IndexOutOfRangeException final : public RuntimeException {
public:
  template <class Container>
  inline IndexOutOfRangeException(
    Container const& indexedContainer,
    index_t index
  ) : RuntimeException(U"Index out of range"s), // TODO: format exception info
      m_container(&indexedContainer),
      m_containerType(typeid(Container)),
      m_containerSize(GetContainerSize(indexedContainer)),
      m_index(index)
  {}

  inline ~IndexOutOfRangeException() override = default;

public:
  inline TypeInfo const& GetExceptionType() const noexcept override {
    return typeid(IndexOutOfRangeException);
  }

  inline void const* GetIndexedContainer() const noexcept {
    return m_container;
  }

  inline TypeInfo const& GetIndexedContainerType() const noexcept {
    return m_containerType;
  }

  inline size_t GetIndexedContainerSize() const noexcept {
    return m_containerSize;
  }

  inline index_t GetIndex() const noexcept {
    return m_index;
  }

private:
  // Container we tried to index:
  void const* const m_container;
  TypeInfo const& m_containerType;
  size_t const m_containerSize;

  index_t const m_index;
};

class IteratorOutOfRangeException final : public RuntimeException {
public:
  template <class Container>
  inline IteratorOutOfRangeException(
    Container const& indexedContainer,
    index_t index
  ) : RuntimeException(U"Iterator out of range"s), // TODO: format exception info
      m_container(&indexedContainer),
      m_containerType(typeid(Container)),
      m_containerSize(GetContainerSize(indexedContainer)),
      m_index(index)
  {}

  inline ~IteratorOutOfRangeException() override = default;

public:
  inline TypeInfo const& GetExceptionType() const noexcept override {
    return typeid(IndexOutOfRangeException);
  }

  inline void const* GetIteratedContainer() const noexcept {
    return m_container;
  }

  inline TypeInfo const& GetIteratedContainerType() const noexcept {
    return m_containerType;
  }

  inline size_t GetIteratedContainerSize() const noexcept {
    return m_containerSize;
  }

  inline size_t GetIndex() const noexcept {
    return m_index;
  }

private:
  // Container we tried to index:
  void const* const m_container;
  TypeInfo const& m_containerType;
  size_t const m_containerSize;

  size_t const m_index;
};

template <class Container, typename T = Container::ItemType>
class BasicIterator {
  friend Container;

protected:
  constexpr BasicIterator(Container* container, size_t index) noexcept
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

  constexpr BasicIterator& operator++() noexcept {
    ++m_index;
    return *this;
  }

  constexpr BasicIterator operator++(int) noexcept {
    BasicIterator old = *this;
    ++(*this);
    return old;
  }

  constexpr BasicIterator& operator--() noexcept {
    --m_index;
    return *this;
  }

  constexpr BasicIterator operator--(int) noexcept {
    BasicIterator old = *this;
    --(*this);
    return old;
  }

  constexpr BasicIterator& operator+=(ssize_t diff) noexcept {
    m_index += diff;
    return *this;
  }

  constexpr BasicIterator& operator-=(ssize_t diff) noexcept {
    m_index -= diff;
    return *this;
  }

  constexpr bool operator==(BasicIterator const& other) const noexcept {
    return other.m_container == m_container && other.m_index == m_index;
  }

  constexpr bool operator!=(BasicIterator const&) const noexcept = default;

protected:
  constexpr T* GetPtr() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

protected:
  Container* const m_container;
  size_t m_index;
};

template <class Container, typename T = Container::ItemType>
class BasicConstIterator {
  friend Container;

protected:
  constexpr BasicConstIterator(Container const* container, size_t index) noexcept
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

  constexpr BasicConstIterator& operator++() noexcept {
    ++m_index;
    return *this;
  }

  constexpr BasicConstIterator operator++(int) noexcept {
    BasicConstIterator old = *this;
    ++(*this);
    return old;
  }

  constexpr BasicConstIterator& operator--() noexcept {
    --m_index;
    return *this;
  }

  constexpr BasicConstIterator operator--(int) noexcept {
    BasicConstIterator old = *this;
    --(*this);
    return old;
  }

  constexpr BasicConstIterator& operator+=(ssize_t diff) noexcept {
    m_index += diff;
    return *this;
  }

  constexpr BasicConstIterator& operator-=(ssize_t diff) noexcept {
    m_index -= diff;
    return *this;
  }

  constexpr bool operator==(BasicConstIterator const& other) const noexcept {
    return other.m_container == m_container && other.m_index == m_index;
  }

  constexpr bool operator!=(BasicConstIterator const&) const noexcept = default;

protected:
  constexpr T const* GetPtr() const noexcept {
    return m_container->_IteratorIndex(m_index);
  }

protected:
  Container const* const m_container;
  size_t m_index;
};
