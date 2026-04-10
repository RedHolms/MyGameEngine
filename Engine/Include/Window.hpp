#pragma once

#include "Core/String.hpp"
#include "Memory/UniquePtr.hpp"

class Window {
public:
  Window(String const& title, uint width, uint height);
  ~Window();

public:

private:
  struct Data;
  UniquePtr<Data> m;
};
