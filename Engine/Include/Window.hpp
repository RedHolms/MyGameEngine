#pragma once

#include "Core/String.hpp"
#include "Utils/PImpl.hpp"

class Window {
public:
  Window(String const& title, uint width, uint height);
  ~Window();

public:

private:
  DECLARE_PIMPL(8);
};
