/**
 * WinPR: Windows Portable Runtime
 * String Manipulation (CRT)
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2016 David PHAM-VAN <d.phamvan@inuvika.com>
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

#ifndef UZI_CRT_STRING_H
#define UZI_CRT_STRING_H

#include <wchar.h>
#include <string.h>
#include <uzi/winpr.h>
#include <uzi/wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

#define CP_UTF8				65001

UZI_API char* _strdup(const char* strSource);
UZI_API WCHAR* _wcsdup(const WCHAR* strSource);

UZI_API int _stricmp(const char* string1, const char* string2);
UZI_API int _strnicmp(const char* string1, const char* string2, size_t count);

UZI_API int _wcscmp(const WCHAR* string1, const WCHAR* string2);

UZI_API size_t _wcslen(const WCHAR* str);
UZI_API WCHAR* _wcschr(const WCHAR* str, WCHAR c);
UZI_API WCHAR* _wcsrchr(const WCHAR* str, WCHAR c);

UZI_API char* strtok_s(char* strToken, const char* strDelimit,
                         char** context);
UZI_API WCHAR* wcstok_s(WCHAR* strToken, const WCHAR* strDelimit,
                          WCHAR** context);

#else

#define _wcscmp		wcscmp
#define _wcslen		wcslen
#define _wcschr		wcschr
#define _wcsrchr	wcsrchr

#endif

#ifndef _WIN32

#define	 sprintf_s	snprintf
#define	 _snprintf	snprintf
#define _scprintf(...) snprintf(NULL, 0, __VA_ARGS__)

#endif

#ifdef __cplusplus
}
#endif

#endif /* UZI_CRT_STRING_H */
