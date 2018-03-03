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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <uzi/handle.h>

#include <uzi/thread.h>

/* Thread local storage keyword define */
#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define UZI_TLS __thread
#else
#define UZI_TLS __declspec(thread)
#endif
#elif !defined(__IOS__)
#define UZI_TLS __thread
#else
//#warning "Target iOS does not support Thread Local Storage!"
//#warning "Multi Instance support is disabled!"
#define UZI_TLS
#endif

#ifndef _WIN32

#include <pthread.h>

DWORD TlsAlloc(void)
{
	pthread_key_t key;

	if (pthread_key_create(&key, NULL) != 0)
		return TLS_OUT_OF_INDEXES;

	return key;
}

LPVOID TlsGetValue(DWORD dwTlsIndex)
{
	LPVOID value;
	pthread_key_t key;
	key = (pthread_key_t) dwTlsIndex;
	value = (LPVOID) pthread_getspecific(key);
	return value;
}

BOOL TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
	pthread_key_t key;
	key = (pthread_key_t) dwTlsIndex;
	pthread_setspecific(key, lpTlsValue);
	return TRUE;
}

BOOL TlsFree(DWORD dwTlsIndex)
{
	pthread_key_t key;
	key = (pthread_key_t) dwTlsIndex;
	pthread_key_delete(key);
	return TRUE;
}

#endif

