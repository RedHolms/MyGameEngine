#pragma once

namespace Intrinsics {

namespace Detail {

extern "C" __declspec(noreturn) void __fastfail(uint);
extern "C" void __debugbreak();

} // namespace Detail

[[noreturn]] forceinline void FailFast(uint failCode) {
  Detail::__fastfail(failCode);
}

forceinline void DebugBreak() {
  Detail::__debugbreak();
}

} // namespace Intrinsics
