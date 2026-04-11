#pragma once

template <class Data, size_t StorageSize, typename AlignAs = uint64_t>
class PImpl {
public:
  constexpr PImpl() noexcept {
    static_assert(sizeof(Data) <= StorageSize);
    new (_GetData()) Data ();
  }

  constexpr ~PImpl() noexcept {
    _GetData()->~Data();
  }

  constexpr PImpl(PImpl const&) noexcept;
  constexpr PImpl(PImpl&&) noexcept;
  constexpr PImpl& operator=(PImpl const&) noexcept;
  constexpr PImpl& operator=(PImpl&&) noexcept;

public:
  constexpr Data* operator->() noexcept {
    return _GetData();
  }

  constexpr Data const* operator->() const noexcept {
    return _GetData();
  }

  constexpr operator Data*() noexcept {
    return _GetData();
  }

  constexpr operator Data const*() const noexcept {
    return _GetData();
  }

private:
  constexpr Data* _GetData() noexcept {
    return reinterpret_cast<Data*>(&m_storage[0]);
  }

  constexpr Data const* _GetData() const noexcept {
    return reinterpret_cast<Data const*>(&m_storage[0]);
  }

private:
  alignas(AlignAs) char m_storage[StorageSize] {};
};

#define DECLARE_PIMPL(STORAGE_SIZE) struct Data; PImpl<Data, STORAGE_SIZE> m
