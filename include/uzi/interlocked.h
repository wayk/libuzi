/**
 * WinPR: Windows Portable Runtime
 * Interlocked Singly-Linked Lists
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

#ifndef UZI_INTERLOCKED_H
#define UZI_INTERLOCKED_H

#include <uzi/spec.h>
#include <uzi/winpr.h>
#include <uzi/wtypes.h>
#include <uzi/platform.h>

#ifdef UZI_MONO_CONFLICT
#define InterlockedCompareExchange64 winpr_InterlockedCompareExchange64
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) \
	    ((type *)(((ULONG_PTR) address) - (ULONG_PTR)(&(((type *) 0)->field))))
#endif

typedef struct _UZI_LIST_ENTRY UZI_LIST_ENTRY;
typedef struct _UZI_LIST_ENTRY* UZI_PLIST_ENTRY;

struct _UZI_LIST_ENTRY
{
	UZI_PLIST_ENTRY Flink;
	UZI_PLIST_ENTRY Blink;
};

typedef struct _UZI_SINGLE_LIST_ENTRY UZI_SINGLE_LIST_ENTRY;
typedef struct _UZI_SINGLE_LIST_ENTRY* UZI_PSINGLE_LIST_ENTRY;

struct _UZI_SINGLE_LIST_ENTRY
{
	UZI_PSINGLE_LIST_ENTRY Next;
};

typedef struct UZI_LIST_ENTRY32
{
	DWORD Flink;
	DWORD Blink;
} UZI_LIST_ENTRY32;
typedef UZI_LIST_ENTRY32 *UZI_PLIST_ENTRY32;

typedef struct UZI_LIST_ENTRY64
{
	ULONGLONG Flink;
	ULONGLONG Blink;
} UZI_LIST_ENTRY64;
typedef UZI_LIST_ENTRY64 *UZI_PLIST_ENTRY64;

#ifdef _WIN64

typedef struct _UZI_SLIST_ENTRY *UZI_PSLIST_ENTRY;
typedef struct DECLSPEC_ALIGN(16) _UZI_SLIST_ENTRY
{
	    UZI_PSLIST_ENTRY Next;
} UZI_SLIST_ENTRY;

#else  /* _WIN64 */

#define UZI_SLIST_ENTRY UZI_SINGLE_LIST_ENTRY
#define _UZI_SLIST_ENTRY _UZI_SINGLE_LIST_ENTRY
#define UZI_PSLIST_ENTRY UZI_PSINGLE_LIST_ENTRY

#endif /* _WIN64 */

#ifdef _WIN64

typedef union DECLSPEC_ALIGN(16) _UZI_SLIST_HEADER
{
	struct
	{
		ULONGLONG Alignment;
		ULONGLONG Region;
	} DUMMYSTRUCTNAME;

	struct
	{
		ULONGLONG Depth:16;
		ULONGLONG Sequence:9;
		ULONGLONG NextEntry:39;
		ULONGLONG HeaderType:1;
		ULONGLONG Init:1;
		ULONGLONG Reserved:59;
		ULONGLONG Region:3;
	} Header8;

	struct
	{
		ULONGLONG Depth:16;
		ULONGLONG Sequence:48;
		ULONGLONG HeaderType:1;
		ULONGLONG Reserved:3;
		ULONGLONG NextEntry:60;
	} HeaderX64;
} UZI_SLIST_HEADER, *UZI_PSLIST_HEADER;

#else  /* _WIN64 */

typedef union _UZI_SLIST_HEADER
{
	ULONGLONG Alignment;

	struct
	{
		UZI_SLIST_ENTRY Next;
		WORD Depth;
		WORD Sequence;
	} DUMMYSTRUCTNAME;
} UZI_SLIST_HEADER, *UZI_PSLIST_HEADER;

#endif /* _WIN64 */

/* Singly-Linked List */

UZI_API VOID InitializeSListHead(UZI_PSLIST_HEADER ListHead);

UZI_API UZI_PSLIST_ENTRY InterlockedPushEntrySList(UZI_PSLIST_HEADER ListHead, UZI_PSLIST_ENTRY ListEntry);
UZI_API UZI_PSLIST_ENTRY InterlockedPushListSListEx(UZI_PSLIST_HEADER ListHead, UZI_PSLIST_ENTRY List, UZI_PSLIST_ENTRY ListEnd, ULONG Count);
UZI_API UZI_PSLIST_ENTRY InterlockedPopEntrySList(UZI_PSLIST_HEADER ListHead);
UZI_API UZI_PSLIST_ENTRY InterlockedFlushSList(UZI_PSLIST_HEADER ListHead);

UZI_API USHORT QueryDepthSList(UZI_PSLIST_HEADER ListHead);

UZI_API LONG InterlockedIncrement(LONG volatile *Addend);
UZI_API LONG InterlockedDecrement(LONG volatile *Addend);

UZI_API LONG InterlockedExchange(LONG volatile *Target, LONG Value);
UZI_API LONG InterlockedExchangeAdd(LONG volatile *Addend, LONG Value);

UZI_API LONG InterlockedCompareExchange(LONG volatile *Destination, LONG Exchange, LONG Comperand);

UZI_API PVOID InterlockedCompareExchangePointer(PVOID volatile *Destination, PVOID Exchange, PVOID Comperand);

#else /* _WIN32 */
#define UZI_LIST_ENTRY LIST_ENTRY
#define _UZI_LIST_ENTRY _LIST_ENTRY
#define UZI_PLIST_ENTRY PLIST_ENTRY

#define UZI_SINGLE_LIST_ENTRY SINGLE_LIST_ENTRY
#define _UZI_SINGLE_LIST_ENTRY _SINGLE_LIST_ENTRY
#define UZI_PSINGLE_LIST_ENTRY PSINGLE_LIST_ENTRY

#define UZI_SLIST_ENTRY SLIST_ENTRY
#define _UZI_SLIST_ENTRY _SLIST_ENTRY
#define UZI_PSLIST_ENTRY PSLIST_ENTRY

#define UZI_SLIST_HEADER SLIST_HEADER
#define _UZI_SLIST_HEADER _SLIST_HEADER
#define UZI_PSLIST_HEADER PSLIST_HEADER

#endif /* _WIN32 */

#if (!defined(_WIN32) || (defined(_WIN32) && (_WIN32_WINNT < 0x0502) && !defined(InterlockedCompareExchange64)))
#define UZI_INTERLOCKED_COMPARE_EXCHANGE64	1
#endif

#ifdef UZI_INTERLOCKED_COMPARE_EXCHANGE64

UZI_API LONGLONG InterlockedCompareExchange64(LONGLONG volatile *Destination, LONGLONG Exchange, LONGLONG Comperand);

#endif

/* Doubly-Linked List */

UZI_API VOID InitializeListHead(UZI_PLIST_ENTRY ListHead);

UZI_API BOOL IsListEmpty(const UZI_LIST_ENTRY* ListHead);

UZI_API BOOL RemoveEntryList(UZI_PLIST_ENTRY Entry);

UZI_API VOID InsertHeadList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY Entry);
UZI_API UZI_PLIST_ENTRY RemoveHeadList(UZI_PLIST_ENTRY ListHead);

UZI_API VOID InsertTailList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY Entry);
UZI_API UZI_PLIST_ENTRY RemoveTailList(UZI_PLIST_ENTRY ListHead);
UZI_API VOID AppendTailList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY ListToAppend);

UZI_API VOID PushEntryList(UZI_PSINGLE_LIST_ENTRY ListHead, UZI_PSINGLE_LIST_ENTRY Entry);
UZI_API UZI_PSINGLE_LIST_ENTRY PopEntryList(UZI_PSINGLE_LIST_ENTRY ListHead);

#ifdef __cplusplus
}
#endif

#endif /* UZI_INTERLOCKED_H */

