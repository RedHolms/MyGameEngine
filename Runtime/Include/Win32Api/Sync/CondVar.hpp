#pragma once

namespace Win32::CondVar {

// Win32 name: SleepConditionVariableSRW
bool SleepSRW(void** condVarPtr, void** srwLockPtr, uint32_t timeout, uint32_t flags);

// Win32 name: WakeAllConditionVariable
void WakeAll(void** condVarPtr);

// Win32 name: WakeConditionVariable
void WakeOne(void** condVarPtr);

} // namespace Win32::CondVar
