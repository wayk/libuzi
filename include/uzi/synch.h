/**
 * WinPR: Windows Portable Runtime
 * Synchronization Functions
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2014 Thincast Technologies GmbH
 * Copyright 2014 Norbert Federa <norbert.federa@thincast.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UZI_SYNCH_H
#define UZI_SYNCH_H

#include <uzi/uzi.h>
#include <uzi/wtypes.h>
#include <uzi/error.h>
#include <uzi/handle.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

/* Mutex */

UZI_API HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
UZI_API HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName);

UZI_API BOOL ReleaseMutex(HANDLE hMutex);

/* Semaphore */

UZI_API HANDLE CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName);
UZI_API HANDLE CreateSemaphoreW(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName);

UZI_API BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);

/* Event */

UZI_API HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
UZI_API HANDLE CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);

UZI_API BOOL SetEvent(HANDLE hEvent);
UZI_API BOOL ResetEvent(HANDLE hEvent);

/* Critical Section */

#if defined(__linux__)
/**
 * Linux NPTL thread synchronization primitives are implemented using
 * the futex system calls ... we can't beat futex with a spin loop.
 */
#define UZI_CRITICAL_SECTION_DISABLE_SPINCOUNT
#endif

typedef struct _RTL_CRITICAL_SECTION
{
	PVOID DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	HANDLE OwningThread;
	HANDLE LockSemaphore;
	ULONG_PTR SpinCount;
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

UZI_API VOID InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
UZI_API BOOL InitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
UZI_API BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);

UZI_API DWORD SetCriticalSectionSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);

UZI_API VOID EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
UZI_API BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

UZI_API VOID LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

UZI_API VOID DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

/* Sleep */

UZI_API VOID Sleep(DWORD dwMilliseconds);
UZI_API DWORD SleepEx(DWORD dwMilliseconds, BOOL bAlertable);

/* Wait */

#define INFINITE		0xFFFFFFFF

#define WAIT_OBJECT_0		0x00000000L
#define WAIT_ABANDONED		0x00000080L

#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT		0x00000102L
#endif

#define WAIT_FAILED		((DWORD) 0xFFFFFFFF)

#define MAXIMUM_WAIT_OBJECTS	64

UZI_API DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
UZI_API DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

/* Waitable Timer */

#define CREATE_WAITABLE_TIMER_MANUAL_RESET		0x00000001

typedef struct _REASON_CONTEXT
{
	ULONG Version;
	DWORD Flags;

	union
	{
		struct
		{
			HMODULE LocalizedReasonModule;
			ULONG LocalizedReasonId;
			ULONG ReasonStringCount;
			LPWSTR* ReasonStrings;
		} Detailed;

		LPWSTR SimpleReasonString;
	} Reason;
} REASON_CONTEXT, *PREASON_CONTEXT;

typedef VOID (*PTIMERAPCROUTINE)(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue);

UZI_API HANDLE CreateWaitableTimerA(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCSTR lpTimerName);
UZI_API HANDLE CreateWaitableTimerW(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCWSTR lpTimerName);

UZI_API HANDLE CreateWaitableTimerExA(LPSECURITY_ATTRIBUTES lpTimerAttributes, LPCSTR lpTimerName, DWORD dwFlags, DWORD dwDesiredAccess);
UZI_API HANDLE CreateWaitableTimerExW(LPSECURITY_ATTRIBUTES lpTimerAttributes, LPCWSTR lpTimerName, DWORD dwFlags, DWORD dwDesiredAccess);

UZI_API BOOL SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER* lpDueTime, LONG lPeriod,
		PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, BOOL fResume);

UZI_API BOOL SetWaitableTimerEx(HANDLE hTimer, const LARGE_INTEGER* lpDueTime, LONG lPeriod,
		PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, PREASON_CONTEXT WakeContext, ULONG TolerableDelay);

UZI_API BOOL CancelWaitableTimer(HANDLE hTimer);

/**
 * Timer-Queue Timer
 */

#define WT_EXECUTEDEFAULT			0x00000000
#define WT_EXECUTEINIOTHREAD			0x00000001
#define WT_EXECUTEINUITHREAD			0x00000002
#define WT_EXECUTEINWAITTHREAD			0x00000004
#define WT_EXECUTEONLYONCE			0x00000008
#define WT_EXECUTELONGFUNCTION			0x00000010
#define WT_EXECUTEINTIMERTHREAD			0x00000020
#define WT_EXECUTEINPERSISTENTIOTHREAD		0x00000040
#define WT_EXECUTEINPERSISTENTTHREAD		0x00000080
#define WT_TRANSFER_IMPERSONATION		0x00000100

typedef VOID (*WAITORTIMERCALLBACK)(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

UZI_API HANDLE CreateTimerQueue(void);
UZI_API BOOL DeleteTimerQueue(HANDLE TimerQueue);
UZI_API BOOL DeleteTimerQueueEx(HANDLE TimerQueue, HANDLE CompletionEvent);

UZI_API BOOL CreateTimerQueueTimer(PHANDLE phNewTimer, HANDLE TimerQueue,
		WAITORTIMERCALLBACK Callback, PVOID Parameter, DWORD DueTime, DWORD Period, ULONG Flags);
UZI_API BOOL ChangeTimerQueueTimer(HANDLE TimerQueue, HANDLE Timer, ULONG DueTime, ULONG Period);
UZI_API BOOL DeleteTimerQueueTimer(HANDLE TimerQueue, HANDLE Timer, HANDLE CompletionEvent);

#endif

#if (defined(_WIN32) && (_WIN32_WINNT < 0x0600))
#define InitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags) InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount)
#endif

#ifndef _RTL_RUN_ONCE_DEF
#define _RTL_RUN_ONCE_DEF

#define RTL_RUN_ONCE_INIT		{ 0 }

#define RTL_RUN_ONCE_CHECK_ONLY		0x00000001
#define RTL_RUN_ONCE_ASYNC		0x00000002
#define RTL_RUN_ONCE_INIT_FAILED	0x00000004

#define RTL_RUN_ONCE_CTX_RESERVED_BITS	2

typedef struct _RTL_RUN_ONCE
{
	PVOID Ptr;
} RTL_RUN_ONCE, *PRTL_RUN_ONCE;

typedef ULONG CALLBACK RTL_RUN_ONCE_INIT_FN (PRTL_RUN_ONCE RunOnce, PVOID Parameter, PVOID* Context);
typedef RTL_RUN_ONCE_INIT_FN *PRTL_RUN_ONCE_INIT_FN;

#endif

#if (!defined(_WIN32)) || (defined(_WIN32) && (_WIN32_WINNT < 0x0600))

/* One-Time Initialization */

#define INIT_ONCE_STATIC_INIT	RTL_RUN_ONCE_INIT

typedef RTL_RUN_ONCE INIT_ONCE;
typedef PRTL_RUN_ONCE PINIT_ONCE;
typedef PRTL_RUN_ONCE LPINIT_ONCE;
typedef BOOL (CALLBACK * PINIT_ONCE_FN)(PINIT_ONCE InitOnce, PVOID Parameter, PVOID* Context);

UZI_API BOOL winpr_InitOnceBeginInitialize(LPINIT_ONCE lpInitOnce, DWORD dwFlags, PBOOL fPending, LPVOID* lpContext);
UZI_API BOOL winpr_InitOnceComplete(LPINIT_ONCE lpInitOnce, DWORD dwFlags, LPVOID lpContext);
UZI_API BOOL winpr_InitOnceExecuteOnce(PINIT_ONCE InitOnce, PINIT_ONCE_FN InitFn, PVOID Parameter, LPVOID* Context);
UZI_API VOID winpr_InitOnceInitialize(PINIT_ONCE InitOnce);

#define InitOnceBeginInitialize winpr_InitOnceBeginInitialize
#define InitOnceComplete winpr_InitOnceComplete
#define InitOnceExecuteOnce winpr_InitOnceExecuteOnce
#define InitOnceInitialize winpr_InitOnceInitialize
#endif

/* Synchronization Barrier */

#if (!defined(_WIN32)) || (defined(_WIN32) && (_WIN32_WINNT < 0x0602) && !defined(_SYNCHAPI_H_))
#define UZI_SYNCHRONIZATION_BARRIER	1
#endif

#ifdef UZI_SYNCHRONIZATION_BARRIER

typedef struct _RTL_BARRIER
{
	DWORD Reserved1;
	DWORD Reserved2;
	ULONG_PTR Reserved3[2];
	DWORD Reserved4;
	DWORD Reserved5;
} RTL_BARRIER, *PRTL_BARRIER;

typedef RTL_BARRIER SYNCHRONIZATION_BARRIER;
typedef PRTL_BARRIER PSYNCHRONIZATION_BARRIER;
typedef PRTL_BARRIER LPSYNCHRONIZATION_BARRIER;

#define SYNCHRONIZATION_BARRIER_FLAGS_SPIN_ONLY		0x01
#define SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY	0x02
#define SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE		0x04

UZI_API BOOL WINAPI winpr_InitializeSynchronizationBarrier(LPSYNCHRONIZATION_BARRIER lpBarrier, LONG lTotalThreads, LONG lSpinCount);
UZI_API BOOL WINAPI winpr_EnterSynchronizationBarrier(LPSYNCHRONIZATION_BARRIER lpBarrier, DWORD dwFlags);
UZI_API BOOL WINAPI winpr_DeleteSynchronizationBarrier(LPSYNCHRONIZATION_BARRIER lpBarrier);

#define InitializeSynchronizationBarrier winpr_InitializeSynchronizationBarrier
#define EnterSynchronizationBarrier winpr_EnterSynchronizationBarrier
#define DeleteSynchronizationBarrier winpr_DeleteSynchronizationBarrier

#endif

/* Extended API */

UZI_API VOID USleep(DWORD dwMicroseconds);

UZI_API HANDLE CreateFileDescriptorEventW(LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset, BOOL bInitialState, int FileDescriptor, ULONG mode);
UZI_API HANDLE CreateFileDescriptorEventA(LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset, BOOL bInitialState, int FileDescriptor, ULONG mode);

UZI_API HANDLE CreateWaitObjectEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset, BOOL bInitialState, void* pObject);

UZI_API int GetEventFileDescriptor(HANDLE hEvent);
UZI_API int SetEventFileDescriptor(HANDLE hEvent, int FileDescriptor, ULONG mode);

UZI_API void* GetEventWaitObject(HANDLE hEvent);

#ifdef __cplusplus
}
#endif

#endif /* UZI_SYNCH_H */

