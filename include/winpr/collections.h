/**
 * WinPR: Windows Portable Runtime
 * Collections
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

#ifndef WINPR_COLLECTIONS_H
#define WINPR_COLLECTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winpr/winpr.h>
#include <winpr/wtypes.h>

#include <winpr/crt.h>
#include <winpr/synch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*OBJECT_NEW_FN)(void);
typedef void (*OBJECT_INIT_FN)(void* obj);
typedef void (*OBJECT_UNINIT_FN)(void* obj);
typedef void (*OBJECT_FREE_FN)(void* obj);
typedef BOOL (*OBJECT_EQUALS_FN)(void* objA, void* objB);

struct _wObject
{
	OBJECT_NEW_FN fnObjectNew;
	OBJECT_INIT_FN fnObjectInit;
	OBJECT_UNINIT_FN fnObjectUninit;
	OBJECT_FREE_FN fnObjectFree;
	OBJECT_EQUALS_FN fnObjectEquals;
};
typedef struct _wObject wObject;

/* System.Collections.Queue */

struct _wQueue
{
	int capacity;
	int growthFactor;
	BOOL synchronized;

	int head;
	int tail;
	int size;
	void** array;
	CRITICAL_SECTION lock;
	HANDLE event;

	wObject object;
};
typedef struct _wQueue wQueue;

WINPR_API int Queue_Count(wQueue* queue);

WINPR_API void Queue_Lock(wQueue* queue);
WINPR_API void Queue_Unlock(wQueue* queue);

WINPR_API HANDLE Queue_Event(wQueue* queue);

#define Queue_Object(_queue)	(&_queue->object)

WINPR_API void Queue_Clear(wQueue* queue);

WINPR_API BOOL Queue_Contains(wQueue* queue, void* obj);

WINPR_API BOOL Queue_Enqueue(wQueue* queue, void* obj);
WINPR_API void* Queue_Dequeue(wQueue* queue);

WINPR_API void* Queue_Peek(wQueue* queue);

WINPR_API wQueue* Queue_New(BOOL synchronized, int capacity, int growthFactor);
WINPR_API void Queue_Free(wQueue* queue);

/* System.Collections.Stack */

struct _wStack
{
	int size;
	int capacity;
	void** array;
	CRITICAL_SECTION lock;
	BOOL synchronized;
	wObject object;
};
typedef struct _wStack wStack;

WINPR_API int Stack_Count(wStack* stack);
WINPR_API BOOL Stack_IsSynchronized(wStack* stack);

#define Stack_Object(_stack)	(&_stack->object)

WINPR_API void Stack_Clear(wStack* stack);
WINPR_API BOOL Stack_Contains(wStack* stack, void* obj);

WINPR_API void Stack_Push(wStack* stack, void* obj);
WINPR_API void* Stack_Pop(wStack* stack);

WINPR_API void* Stack_Peek(wStack* stack);

WINPR_API wStack* Stack_New(BOOL synchronized);
WINPR_API void Stack_Free(wStack* stack);

/* System.Collections.ArrayList */

struct _wArrayList
{
	int capacity;
	int growthFactor;
	BOOL synchronized;

	int size;
	void** array;
	CRITICAL_SECTION lock;

	wObject object;
};
typedef struct _wArrayList wArrayList;

WINPR_API int ArrayList_Capacity(wArrayList* arrayList);
WINPR_API int ArrayList_Count(wArrayList* arrayList);
WINPR_API int ArrayList_Items(wArrayList* arrayList, ULONG_PTR** ppItems);
WINPR_API BOOL ArrayList_IsFixedSized(wArrayList* arrayList);
WINPR_API BOOL ArrayList_IsReadOnly(wArrayList* arrayList);
WINPR_API BOOL ArrayList_IsSynchronized(wArrayList* arrayList);

WINPR_API void ArrayList_Lock(wArrayList* arrayList);
WINPR_API void ArrayList_Unlock(wArrayList* arrayList);

WINPR_API void* ArrayList_GetItem(wArrayList* arrayList, int index);
WINPR_API void ArrayList_SetItem(wArrayList* arrayList, int index, void* obj);

#define ArrayList_Object(_arrayList)	(&_arrayList->object)

WINPR_API void ArrayList_Clear(wArrayList* arrayList);
WINPR_API BOOL ArrayList_Contains(wArrayList* arrayList, void* obj);

WINPR_API int ArrayList_Add(wArrayList* arrayList, void* obj);
WINPR_API BOOL ArrayList_Insert(wArrayList* arrayList, int index, void* obj);

WINPR_API BOOL ArrayList_Remove(wArrayList* arrayList, void* obj);
WINPR_API BOOL ArrayList_RemoveAt(wArrayList* arrayList, int index);

WINPR_API int ArrayList_IndexOf(wArrayList* arrayList, void* obj, int startIndex, int count);
WINPR_API int ArrayList_LastIndexOf(wArrayList* arrayList, void* obj, int startIndex, int count);

WINPR_API wArrayList* ArrayList_New(BOOL synchronized);
WINPR_API void ArrayList_Free(wArrayList* arrayList);

/* System.Collections.Specialized.ListDictionary */

typedef struct _wListDictionaryItem wListDictionaryItem;

struct _wListDictionaryItem
{
	void* key;
	void* value;

	wListDictionaryItem* next;
};

struct _wListDictionary
{
	BOOL synchronized;
	CRITICAL_SECTION lock;

	wListDictionaryItem* head;
	wObject objectKey;
	wObject objectValue;
};
typedef struct _wListDictionary wListDictionary;

#define ListDictionary_KeyObject(_dictionary)	(&_dictionary->objectKey)
#define ListDictionary_ValueObject(_dictionary)	(&_dictionary->objectValue)

WINPR_API int ListDictionary_Count(wListDictionary* listDictionary);

WINPR_API void ListDictionary_Lock(wListDictionary* listDictionary);
WINPR_API void ListDictionary_Unlock(wListDictionary* listDictionary);

WINPR_API BOOL ListDictionary_Add(wListDictionary* listDictionary, void* key, void* value);
WINPR_API void* ListDictionary_Remove(wListDictionary* listDictionary, void* key);
WINPR_API void* ListDictionary_Remove_Head(wListDictionary* listDictionary);
WINPR_API void ListDictionary_Clear(wListDictionary* listDictionary);

WINPR_API BOOL ListDictionary_Contains(wListDictionary* listDictionary, void* key);
WINPR_API int ListDictionary_GetKeys(wListDictionary* listDictionary, ULONG_PTR** ppKeys);

WINPR_API void* ListDictionary_GetItemValue(wListDictionary* listDictionary, void* key);
WINPR_API BOOL ListDictionary_SetItemValue(wListDictionary* listDictionary, void* key, void* value);

WINPR_API wListDictionary* ListDictionary_New(BOOL synchronized);
WINPR_API void ListDictionary_Free(wListDictionary* listDictionary);

/* System.Collections.Generic.LinkedList<T> */

typedef struct _wLinkedListItem wLinkedListNode;

struct _wLinkedListItem
{
	void* value;
	wLinkedListNode* prev;
	wLinkedListNode* next;
};

struct _wLinkedList
{
	int count;
	int initial;
	wLinkedListNode* head;
	wLinkedListNode* tail;
	wLinkedListNode* current;
};
typedef struct _wLinkedList wLinkedList;

WINPR_API int LinkedList_Count(wLinkedList* list);
WINPR_API void* LinkedList_First(wLinkedList* list);
WINPR_API void* LinkedList_Last(wLinkedList* list);

WINPR_API BOOL LinkedList_Contains(wLinkedList* list, void* value);
WINPR_API void LinkedList_Clear(wLinkedList* list);

WINPR_API BOOL LinkedList_AddFirst(wLinkedList* list, void* value);
WINPR_API BOOL LinkedList_AddLast(wLinkedList* list, void* value);

WINPR_API BOOL LinkedList_Remove(wLinkedList* list, void* value);
WINPR_API void LinkedList_RemoveFirst(wLinkedList* list);
WINPR_API void LinkedList_RemoveLast(wLinkedList* list);

WINPR_API void LinkedList_Enumerator_Reset(wLinkedList* list);
WINPR_API void* LinkedList_Enumerator_Current(wLinkedList* list);
WINPR_API BOOL LinkedList_Enumerator_MoveNext(wLinkedList* list);

WINPR_API wLinkedList* LinkedList_New(void);
WINPR_API void LinkedList_Free(wLinkedList* list);

/* Countdown Event */

struct _wCountdownEvent
{
	DWORD count;
	CRITICAL_SECTION lock;
	HANDLE event;
	DWORD initialCount;
};
typedef struct _wCountdownEvent wCountdownEvent;

WINPR_API DWORD CountdownEvent_CurrentCount(wCountdownEvent* countdown);
WINPR_API DWORD CountdownEvent_InitialCount(wCountdownEvent* countdown);
WINPR_API BOOL CountdownEvent_IsSet(wCountdownEvent* countdown);
WINPR_API HANDLE CountdownEvent_WaitHandle(wCountdownEvent* countdown);

WINPR_API void CountdownEvent_AddCount(wCountdownEvent* countdown, DWORD signalCount);
WINPR_API BOOL CountdownEvent_Signal(wCountdownEvent* countdown, DWORD signalCount);
WINPR_API void CountdownEvent_Reset(wCountdownEvent* countdown, DWORD count);

WINPR_API wCountdownEvent* CountdownEvent_New(DWORD initialCount);
WINPR_API void CountdownEvent_Free(wCountdownEvent* countdown);

#ifdef __cplusplus
}
#endif

#endif /* WINPR_COLLECTIONS_H */
