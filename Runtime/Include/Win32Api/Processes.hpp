#pragma once

namespace Win32 {

using ProcessID = uint32_t;

namespace CurrentProcess {

// Win32 name: ExitProcess
[[noreturn]] void Exit(uint exitCode);

// Win32 name: GetCurrentProcessId
ProcessID GetID();

} // namespace CurrentProcess

} // namespace Win32
