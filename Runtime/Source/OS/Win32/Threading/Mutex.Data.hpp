#pragma once

#include "Threading/Mutex.hpp"

struct Mutex::Data {
  void* srwLock = nullptr;
};
