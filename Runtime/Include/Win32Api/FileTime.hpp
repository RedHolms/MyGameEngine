#pragma once

namespace Win32 {

// Win32 name: FILETIME
union FileTime {
  struct {
    uint32_t lowDateTime;
    uint32_t highDateTime;
  };
  uint64_t fullDateTime;
};

} // namespace Win32
