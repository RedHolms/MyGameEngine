#pragma once

extern "C++" struct __type_info_node;

extern "C" struct __std_type_info_data {
  const char* undecoratedName;
  const char decoratedName[1];

  __std_type_info_data() = delete;
  __std_type_info_data(__std_type_info_data const&) = delete;
  __std_type_info_data(__std_type_info_data&&) = delete;
  __std_type_info_data& operator=(__std_type_info_data const&) = delete;
  __std_type_info_data& operator=(__std_type_info_data&&) = delete;
};

extern "C++" class type_info {
public:
  type_info(const type_info&) = delete;
  type_info& operator=(const type_info&) = delete;

  virtual ~type_info() noexcept;

private:
  mutable __std_type_info_data m_data;
};
