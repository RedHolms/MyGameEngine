#pragma once

template <class T>
concept TLockable = requires(T obj) {
  obj.Lock();
  obj.Unlock();
};
