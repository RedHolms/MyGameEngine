#include "Marcoses/Helpers.hpp"
#include "Marcoses/Platform.hpp"
#include <stdio.h>

#define STRINGIFY_(X) #X
#define STRINGIFY(X)  STRINGIFY_(X)

int main() {
  printf(
    "%s;%s;%s;%s;%s;%s",
    STRINGIFY(CURRENT_OS_CODENAME),
    CURRENT_OS_NAME,
    STRINGIFY(CURRENT_ABI_FAMILY_CODENAME),
    CURRENT_ABI_FAMILY_NAME,
    STRINGIFY(CURRENT_ARCH_CODENAME),
    CURRENT_ARCH_NAME
  );
  return 0;
}
