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

#ifndef UZI_COLLECTIONS_H
#define UZI_COLLECTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uzi/uzi.h>
#include <uzi/wtypes.h>

#include <uzi/crt.h>
#include <uzi/synch.h>

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

UZI_API int ListDictionary_Count(wListDictionary* listDictionary);

UZI_API void ListDictionary_Lock(wListDictionary* listDictionary);
UZI_API void ListDictionary_Unlock(wListDictionary* listDictionary);

UZI_API BOOL ListDictionary_Add(wListDictionary* listDictionary, void* key, void* value);
UZI_API void* ListDictionary_Remove(wListDictionary* listDictionary, void* key);
UZI_API void* ListDictionary_Remove_Head(wListDictionary* listDictionary);
UZI_API void ListDictionary_Clear(wListDictionary* listDictionary);

UZI_API BOOL ListDictionary_Contains(wListDictionary* listDictionary, void* key);
UZI_API int ListDictionary_GetKeys(wListDictionary* listDictionary, ULONG_PTR** ppKeys);

UZI_API void* ListDictionary_GetItemValue(wListDictionary* listDictionary, void* key);
UZI_API BOOL ListDictionary_SetItemValue(wListDictionary* listDictionary, void* key, void* value);

UZI_API wListDictionary* ListDictionary_New(BOOL synchronized);
UZI_API void ListDictionary_Free(wListDictionary* listDictionary);

#ifdef __cplusplus
}
#endif

#endif /* UZI_COLLECTIONS_H */
