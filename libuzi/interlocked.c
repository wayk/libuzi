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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <uzi/synch.h>
#include <uzi/handle.h>

#include <uzi/interlocked.h>

/* Singly-Linked List */

#ifndef _WIN32

#include <stdio.h>
#include <stdlib.h>

VOID InitializeSListHead(UZI_PSLIST_HEADER ListHead)
{
#ifdef _WIN64
	ListHead->s.Alignment = 0;
	ListHead->s.Region = 0;
	ListHead->Header8.Init = 1;
#else
	ListHead->Alignment = 0;
#endif
}

UZI_PSLIST_ENTRY InterlockedPushEntrySList(UZI_PSLIST_HEADER ListHead, UZI_PSLIST_ENTRY ListEntry)
{
	UZI_SLIST_HEADER old;
	UZI_SLIST_HEADER newHeader;

#ifdef _WIN64
	newHeader.HeaderX64.NextEntry = (((ULONG_PTR) ListEntry) >> 4);

	while (1)
	{
		old = *ListHead;

		ListEntry->Next = (PSLIST_ENTRY) (((ULONG_PTR) old.HeaderX64.NextEntry) << 4);

		newHeader.HeaderX64.Depth = old.HeaderX64.Depth + 1;
		newHeader.HeaderX64.Sequence = old.HeaderX64.Sequence + 1;

		if (InterlockedCompareExchange64((LONGLONG*) ListHead, newHeader.s.Alignment, old.s.Alignment))
		{
			InterlockedCompareExchange64(&((LONGLONG*) ListHead)[1], newHeader.s.Region, old.s.Region);
			break;
		}
	}

	return (PSLIST_ENTRY) ((ULONG_PTR) old.HeaderX64.NextEntry << 4);
#else
	newHeader.s.Next.Next = ListEntry;

	do
	{
		old = *ListHead;
		ListEntry->Next = old.s.Next.Next;
		newHeader.s.Depth = old.s.Depth + 1;
		newHeader.s.Sequence = old.s.Sequence + 1;
	}
	while(InterlockedCompareExchange64((LONGLONG*) &ListHead->Alignment, newHeader.Alignment, old.Alignment) != old.Alignment);

	return old.s.Next.Next;
#endif
}

UZI_PSLIST_ENTRY InterlockedPushListSListEx(UZI_PSLIST_HEADER ListHead, UZI_PSLIST_ENTRY List, UZI_PSLIST_ENTRY ListEnd, ULONG Count)
{
#ifdef _WIN64

#else

#endif
	return NULL;
}

UZI_PSLIST_ENTRY InterlockedPopEntrySList(UZI_PSLIST_HEADER ListHead)
{
	UZI_SLIST_HEADER old;
	UZI_SLIST_HEADER newHeader;
	UZI_PSLIST_ENTRY entry;

#ifdef _WIN64
	while (1)
	{
		old = *ListHead;

		entry = (PSLIST_ENTRY) (((ULONG_PTR) old.HeaderX64.NextEntry) << 4);

		if (!entry)
			return NULL;

		newHeader.HeaderX64.NextEntry = ((ULONG_PTR) entry->Next) >> 4;
		newHeader.HeaderX64.Depth = old.HeaderX64.Depth - 1;
		newHeader.HeaderX64.Sequence = old.HeaderX64.Sequence - 1;

		if (InterlockedCompareExchange64((LONGLONG*) ListHead, newHeader.s.Alignment, old.s.Alignment))
		{
			InterlockedCompareExchange64(&((LONGLONG*) ListHead)[1], newHeader.s.Region, old.s.Region);
			break;
		}
	}
#else
	do
	{
		old = *ListHead;

		entry = old.s.Next.Next;

		if (!entry)
			return NULL;

		newHeader.s.Next.Next = entry->Next;
		newHeader.s.Depth = old.s.Depth - 1;
		newHeader.s.Sequence = old.s.Sequence + 1;
	}
	while(InterlockedCompareExchange64((LONGLONG*) &ListHead->Alignment, newHeader.Alignment, old.Alignment) != old.Alignment);
#endif
	return entry;
}

UZI_PSLIST_ENTRY InterlockedFlushSList(UZI_PSLIST_HEADER ListHead)
{
	UZI_SLIST_HEADER old;
	UZI_SLIST_HEADER newHeader;

	if (!QueryDepthSList(ListHead))
		return NULL;

#ifdef _WIN64
	newHeader.s.Alignment = 0;
	newHeader.s.Region = 0;
	newHeader.HeaderX64.HeaderType = 1;

	while (1)
	{
		old = *ListHead;
		newHeader.HeaderX64.Sequence = old.HeaderX64.Sequence + 1;

		if (InterlockedCompareExchange64((LONGLONG*) ListHead, newHeader.s.Alignment, old.s.Alignment))
		{
			InterlockedCompareExchange64(&((LONGLONG*) ListHead)[1], newHeader.s.Region, old.s.Region);
			break;
		}
	}

	return (PSLIST_ENTRY) (((ULONG_PTR) old.HeaderX64.NextEntry) << 4);
#else
	newHeader.Alignment = 0;

	do
	{
		old = *ListHead;
		newHeader.s.Sequence = old.s.Sequence + 1;
	}
	while(InterlockedCompareExchange64((LONGLONG*) &ListHead->Alignment, newHeader.Alignment, old.Alignment) != old.Alignment);

	return old.s.Next.Next;
#endif
}

USHORT QueryDepthSList(UZI_PSLIST_HEADER ListHead)
{
#ifdef _WIN64
	return ListHead->HeaderX64.Depth;
#else
	return ListHead->s.Depth;
#endif
}

LONG InterlockedIncrement(LONG volatile *Addend)
{
#ifdef __GNUC__
	return __sync_add_and_fetch(Addend, 1);
#else
	return 0;
#endif
}

LONG InterlockedDecrement(LONG volatile *Addend)
{
#ifdef __GNUC__
	return __sync_sub_and_fetch(Addend, 1);
#else
	return 0;
#endif
}

LONG InterlockedExchange(LONG volatile *Target, LONG Value)
{
#ifdef __GNUC__
	return __sync_val_compare_and_swap(Target, *Target, Value);
#else
	return 0;
#endif
}

LONG InterlockedExchangeAdd(LONG volatile *Addend, LONG Value)
{
#ifdef __GNUC__
	return __sync_fetch_and_add(Addend, Value);
#else
	return 0;
#endif
}

LONG InterlockedCompareExchange(LONG volatile *Destination, LONG Exchange, LONG Comperand)
{
#ifdef __GNUC__
	return __sync_val_compare_and_swap(Destination, Comperand, Exchange);
#else
	return 0;
#endif
}

PVOID InterlockedCompareExchangePointer(PVOID volatile *Destination, PVOID Exchange, PVOID Comperand)
{
#ifdef __GNUC__
	return __sync_val_compare_and_swap(Destination, Comperand, Exchange);
#else
	return 0;
#endif
}

#endif /* _WIN32 */

#if defined(_WIN32) && !defined(UZI_INTERLOCKED_COMPARE_EXCHANGE64)

/* InterlockedCompareExchange64 already defined */

#elif defined(_WIN32) && defined(UZI_INTERLOCKED_COMPARE_EXCHANGE64)

static volatile HANDLE mutex = NULL;

BOOL static_mutex_lock(volatile HANDLE* static_mutex)
{
	if (*static_mutex == NULL)
	{
		HANDLE handle;

		if (!(handle = CreateMutex(NULL, FALSE, NULL)))
			return FALSE;

		if (InterlockedCompareExchangePointer((PVOID*) static_mutex, (PVOID) handle, NULL) != NULL)
			CloseHandle(handle);
	}

	return (WaitForSingleObject(*static_mutex, INFINITE) == WAIT_OBJECT_0);
}

LONGLONG InterlockedCompareExchange64(LONGLONG volatile *Destination, LONGLONG Exchange, LONGLONG Comperand)
{
	LONGLONG previousValue = 0;
	BOOL locked = static_mutex_lock(&mutex);

	previousValue = *Destination;

	if (*Destination == Comperand)
		*Destination = Exchange;

	if (locked)
		ReleaseMutex(mutex);
	else
		fprintf(stderr, "WARNING: InterlockedCompareExchange64 operation might have failed\n");

	return previousValue;
}

#elif (defined(ANDROID) && ANDROID) || (defined(__GNUC__) && !defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))

#include <pthread.h>

static pthread_mutex_t mutex;

LONGLONG InterlockedCompareExchange64(LONGLONG volatile *Destination, LONGLONG Exchange, LONGLONG Comperand)
{
	LONGLONG previousValue = 0;

	pthread_mutex_lock(&mutex);

	previousValue = *Destination;

	if (*Destination == Comperand)
		*Destination = Exchange;

	pthread_mutex_unlock(&mutex);

	return previousValue;
}

#else

LONGLONG InterlockedCompareExchange64(LONGLONG volatile *Destination, LONGLONG Exchange, LONGLONG Comperand)
{
#ifdef __GNUC__
	return __sync_val_compare_and_swap(Destination, Comperand, Exchange);
#else
	return 0;
#endif
}

#endif

/* Doubly-Linked List */

/**
 * Kernel-Mode Basics: Windows Linked Lists:
 * http://www.osronline.com/article.cfm?article=499
 *
 * Singly and Doubly Linked Lists:
 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff563802/
 */

VOID InitializeListHead(UZI_PLIST_ENTRY ListHead)
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

BOOL IsListEmpty(const UZI_LIST_ENTRY* ListHead)
{
	return (BOOL) (ListHead->Flink == ListHead);
}

BOOL RemoveEntryList(UZI_PLIST_ENTRY Entry)
{
	UZI_PLIST_ENTRY OldFlink;
	UZI_PLIST_ENTRY OldBlink;

	OldFlink = Entry->Flink;
	OldBlink = Entry->Blink;
	OldFlink->Blink = OldBlink;
	OldBlink->Flink = OldFlink;

	return (BOOL) (OldFlink == OldBlink);
}

VOID InsertHeadList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY Entry)
{
	UZI_PLIST_ENTRY OldFlink;

	OldFlink = ListHead->Flink;
	Entry->Flink = OldFlink;
	Entry->Blink = ListHead;
	OldFlink->Blink = Entry;
	ListHead->Flink = Entry;
}

UZI_PLIST_ENTRY RemoveHeadList(UZI_PLIST_ENTRY ListHead)
{
	UZI_PLIST_ENTRY Flink;
	UZI_PLIST_ENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;

	return Entry;
}

VOID InsertTailList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY Entry)
{
	UZI_PLIST_ENTRY OldBlink;

	OldBlink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = OldBlink;
	OldBlink->Flink = Entry;
	ListHead->Blink = Entry;
}

UZI_PLIST_ENTRY RemoveTailList(UZI_PLIST_ENTRY ListHead)
{
	UZI_PLIST_ENTRY Blink;
	UZI_PLIST_ENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;

	return Entry;
}

VOID AppendTailList(UZI_PLIST_ENTRY ListHead, UZI_PLIST_ENTRY ListToAppend)
{
	UZI_PLIST_ENTRY ListEnd = ListHead->Blink;

	ListHead->Blink->Flink = ListToAppend;
	ListHead->Blink = ListToAppend->Blink;
	ListToAppend->Blink->Flink = ListHead;
	ListToAppend->Blink = ListEnd;
}

VOID PushEntryList(UZI_PSINGLE_LIST_ENTRY ListHead, UZI_PSINGLE_LIST_ENTRY Entry)
{
	Entry->Next = ListHead->Next;
	ListHead->Next = Entry;
}

UZI_PSINGLE_LIST_ENTRY PopEntryList(UZI_PSINGLE_LIST_ENTRY ListHead)
{
	UZI_PSINGLE_LIST_ENTRY FirstEntry;

	FirstEntry = ListHead->Next;

	if (FirstEntry != NULL)
		ListHead->Next = FirstEntry->Next;

	return FirstEntry;
}
