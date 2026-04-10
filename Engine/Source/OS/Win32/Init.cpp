#include "Windows/WindowsController.hpp"

uint OsSpecificInit() {
  gWindowsController = new WindowsController();
  return 0;
}

void OsSpecificTerminate() {
  delete gWindowsController;
}
