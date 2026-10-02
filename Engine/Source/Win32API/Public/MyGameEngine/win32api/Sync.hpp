#pragma once

#include <MyGameEngine/win32api/Base.hpp>

namespace win32api {
extern "C" {

constexpr DWORD INFINITE = 0xFFFFFFFF;

void Sleep(DWORD milliseconds) _WIN32_FUNC_ATTRS;

typedef void *SRWLOCK, **PSRWLOCK;
typedef void *CONDITION_VARIABLE, **PCONDITION_VARIABLE;

void AcquireSRWLockExclusive(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;
void ReleaseSRWLockExclusive(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;
BOOLEAN TryAcquireSRWLockExclusive(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;
void AcquireSRWLockShared(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;
void ReleaseSRWLockShared(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;
BOOLEAN TryAcquireSRWLockShared(PSRWLOCK SRWLock) _WIN32_FUNC_ATTRS;

constexpr unsigned long CONDITION_VARIABLE_LOCKMODE_SHARED = 0x1;

BOOL SleepConditionVariableSRW(PCONDITION_VARIABLE condvar, PSRWLOCK SRWLock, DWORD milliseconds, unsigned long flags)
  _WIN32_FUNC_ATTRS;
void WakeAllConditionVariable(PCONDITION_VARIABLE condvar) _WIN32_FUNC_ATTRS;
void WakeConditionVariable(PCONDITION_VARIABLE condvar) _WIN32_FUNC_ATTRS;

HANDLE CreateEventW(LPSECURITY_ATTRIBUTES eventAttributes, BOOL manualReset, BOOL initialState, const wchar_t* name)
  _WIN32_FUNC_ATTRS;

} // extern "C"
} // namespace win32api
