#include "Win32Api/Sync/CondVar.hpp"

#include "OS/Win32/Api/DeclSpecs.hpp"

extern "C" {

WIN32_FUNC uint32_t WIN32_API SleepConditionVariableSRW(void**, void**, uint32_t, uint32_t);
WIN32_FUNC void WIN32_API WakeAllConditionVariable(void**);
WIN32_FUNC void WIN32_API WakeConditionVariable(void**);

} // extern "C"

bool Win32::CondVar::SleepSRW(void** condVarPtr, void** srwLockPtr, uint32_t timeout, uint32_t flags) {
  return !!::SleepConditionVariableSRW(condVarPtr, srwLockPtr, timeout, flags);
}

void Win32::CondVar::WakeAll(void** condVarPtr) {
  ::WakeAllConditionVariable(condVarPtr);
}

void Win32::CondVar::WakeOne(void** condVarPtr) {
  ::WakeConditionVariable(condVarPtr);
}
