#pragma once

#include "Base.hpp"
#include "Processes.hpp"

namespace Win32::CurrentProcess {

// Win32 name: ExitProcess
[[noreturn]] void Exit(uint exitCode);

// Win32 name: GetCurrentProcessId
ProcessID GetID();

} // namespace Win32::CurrentProcess
