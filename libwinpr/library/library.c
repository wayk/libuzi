/**
 * WinPR: Windows Portable Runtime
 * Library Loader
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

#include <winpr/crt.h>
#include <winpr/platform.h>

#include <winpr/library.h>

#define TAG "library"

#if !defined(_WIN32) || defined(_UWP)

#ifndef _WIN32

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __MACOSX__
#include <mach-o/dyld.h>
#endif

#endif

HMODULE LoadLibraryA(LPCSTR lpLibFileName)
{
#if defined(_UWP)
	int status;
	HMODULE hModule = NULL;
	WCHAR* filenameW = NULL;

	if (!lpLibFileName)
		return NULL;

	status = ConvertToUnicode(CP_UTF8, 0, lpLibFileName, -1, &filenameW, 0);

	if (status < 1)
		return NULL;

	hModule = LoadPackagedLibrary(filenameW, 0);

	free(filenameW);

	return hModule;
#else
	HMODULE library;

	library = dlopen(lpLibFileName, RTLD_LOCAL | RTLD_LAZY);

	if (!library)
		return NULL;

	return library;
#endif
}

HMODULE LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
#if !defined(_UWP)
	HMODULE library;
	library = dlopen(lpLibFileName, RTLD_LOCAL | RTLD_LAZY);
	return library;
#else
	return (HMODULE)NULL;
#endif
}

#endif

#if !defined(_WIN32) && !defined(__CYGWIN__)

FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC proc;
	proc = dlsym(hModule, lpProcName);
	return proc;
}

BOOL FreeLibrary(HMODULE hLibModule)
{
	int status;
	status = dlclose(hLibModule);

	if (status != 0)
		return FALSE;

	return TRUE;
}

#endif
