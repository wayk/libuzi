/**
 * WinPR: Windows Portable Runtime
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

#ifndef UZI_H
#define UZI_H

#include <uzi/platform.h>

#ifdef UZI_DLL
#if defined _WIN32 || defined __CYGWIN__
#ifdef UZI_EXPORTS
#ifdef __GNUC__
#define UZI_API __attribute__((dllexport))
#else
#define UZI_API __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define UZI_API __attribute__((dllimport))
#else
#define UZI_API __declspec(dllimport)
#endif
#endif
#else
#if __GNUC__ >= 4
#define UZI_API   __attribute__ ((visibility("default")))
#else
#define UZI_API
#endif
#endif
#else /* UZI_DLL */
#define UZI_API
#endif

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


#ifdef _WIN32
#define INLINE	__inline
#else
#define INLINE	inline
#endif

UZI_API void winpr_get_version(int* major, int* minor, int* revision);
UZI_API const char* winpr_get_version_string(void);
UZI_API const char* winpr_get_build_date(void);
UZI_API const char* winpr_get_build_revision(void);
UZI_API const char* winpr_get_build_config(void);

#endif /* UZI_H */
