/**
 * WinPR: Windows Portable Runtime
 * Process Thread Functions
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#ifndef UZI_THREAD_H
#define UZI_THREAD_H

#include <uzi/winpr.h>
#include <uzi/wtypes.h>

#include <uzi/spec.h>
#include <uzi/handle.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

/* Thread */

#define CREATE_SUSPENDED				0x00000004
#define STACK_SIZE_PARAM_IS_A_RESERVATION		0x00010000

UZI_API HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

UZI_API DECLSPEC_NORETURN VOID ExitThread(DWORD dwExitCode);
UZI_API BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);

UZI_API HANDLE _GetCurrentThread(void);
UZI_API DWORD GetCurrentThreadId(void);

UZI_API DWORD ResumeThread(HANDLE hThread);
UZI_API DWORD SuspendThread(HANDLE hThread);
UZI_API BOOL SwitchToThread(void);

UZI_API BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode);

/* Processor */

UZI_API DWORD GetCurrentProcessorNumber(void);

/* Thread-Local Storage */

#define TLS_OUT_OF_INDEXES	((DWORD) 0xFFFFFFFF)

UZI_API DWORD TlsAlloc(void);
UZI_API LPVOID TlsGetValue(DWORD dwTlsIndex);
UZI_API BOOL TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
UZI_API BOOL TlsFree(DWORD dwTlsIndex);

#else

/*
 * GetCurrentProcess / GetCurrentThread cause a conflict on Mac OS X
 */
#define _GetCurrentProcess	GetCurrentProcess
#define _GetCurrentThread	GetCurrentThread

#endif

#ifdef __cplusplus
}
#endif

#endif /* UZI_THREAD_H */

