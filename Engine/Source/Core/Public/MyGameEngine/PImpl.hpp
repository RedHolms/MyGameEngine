#pragma once

#include <utility>

template <typename DataT, size_t MaxImplSize, size_t Alignment = 8>
class PImpl {
public:
  template <typename... ArgsT>
  explicit inline PImpl(ArgsT&&... args) {
    static_assert(sizeof(DataT) <= MaxImplSize);
    static_assert(alignof(DataT) <= Alignment);
    new (GetDataPtr()) DataT (std::forward<ArgsT>(args)...);
  }

  inline ~PImpl() {
    GetDataPtr()->~DataT();
  }

  inline PImpl(PImpl const& other) {
    new (GetDataPtr()) DataT (*other.GetDataPtr());
  }

  inline PImpl(PImpl&& other) noexcept {
    new (GetDataPtr()) DataT (std::move(*other.GetDataPtr()));
  }

  inline PImpl& operator=(PImpl const& other) {
    *GetDataPtr() = *other.GetDataPtr();
    return *this;
  }

  inline PImpl& operator=(PImpl&& other) noexcept {
    *GetDataPtr() = std::move(*other.GetDataPtr());
    return *this;
  }

public:
  inline DataT* operator->() noexcept {
    return GetDataPtr();
  }

  inline DataT const* operator->() const noexcept {
    return GetDataPtr();
  }

  inline DataT* GetDataPtr() noexcept {
    return reinterpret_cast<DataT*>(&m_dataBuffer[0]);
  }

  inline DataT const* GetDataPtr() const noexcept {
    return reinterpret_cast<DataT const*>(&m_dataBuffer[0]);
  }

private:
  alignas(Alignment) char m_dataBuffer[MaxImplSize] {};
};
