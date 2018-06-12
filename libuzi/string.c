/**
 * WinPR: Windows Portable Runtime
 * String Manipulation (CRT)
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

#include <errno.h>
#include <stdio.h>
#include <wctype.h>

#include <uzi/crt.h>

/* String Manipulation (CRT): http://msdn.microsoft.com/en-us/library/f0151s4x.aspx */

#define TAG "crt"

#ifndef _WIN32

char* _strdup(const char* src)
{
	return strdup(src);
}

int _stricmp(const char* string1, const char* string2)
{
	return strcasecmp(string1, string2);
}

int _strnicmp(const char* string1, const char* string2, size_t count)
{
	return strncasecmp(string1, string2, count);
}

/* _wcscmp -> wcscmp */

int _wcscmp(const WCHAR* string1, const WCHAR* string2)
{
	while (*string1 && (*string1 == *string2))
	{
		string1++;
		string2++;
	}

	return *string1 - *string2;
}

/* _wcslen -> wcslen */

size_t _wcslen(const WCHAR* str)
{
	WCHAR* p = (WCHAR*) str;

	if (!p)
		return 0;

	while (*p)
		p++;

	return (p - str);
}

char* strtok_s(char* strToken, const char* strDelimit, char** context)
{
	return strtok_r(strToken, strDelimit, context);
}

#endif

/* Data Conversion: http://msdn.microsoft.com/en-us/library/0heszx3w/ */

#ifndef _WIN32

errno_t _itoa_s(int value, char* buffer, size_t sizeInCharacters, int radix)
{
	int length;

	length = sprintf_s(NULL, 0, "%d", value);

	if (sizeInCharacters < length)
		return -1;

	sprintf_s(buffer, length + 1, "%d", value);

	return 0;
}

#endif
