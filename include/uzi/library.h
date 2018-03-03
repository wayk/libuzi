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

#ifndef UZI_LIBRARY_H
#define UZI_LIBRARY_H

#include <uzi/winpr.h>
#include <uzi/wtypes.h>

#if !defined(_WIN32) || defined(_UWP)

typedef HANDLE DLL_DIRECTORY_COOKIE;

#define LOAD_LIBRARY_SEARCH_APPLICATION_DIR			0x00000200
#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS			0x00001000
#define LOAD_LIBRARY_SEARCH_SYSTEM32				0x00000800
#define LOAD_LIBRARY_SEARCH_USER_DIRS				0x00000400

#ifdef __cplusplus
extern "C" {
#endif

UZI_API DLL_DIRECTORY_COOKIE AddDllDirectory(PCWSTR NewDirectory);
UZI_API BOOL RemoveDllDirectory(DLL_DIRECTORY_COOKIE Cookie);
UZI_API BOOL SetDefaultDllDirectories(DWORD DirectoryFlags);

UZI_API HMODULE LoadLibraryA(LPCSTR lpLibFileName);
UZI_API HMODULE LoadLibraryW(LPCWSTR lpLibFileName);

UZI_API HMODULE LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
UZI_API HMODULE LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

#ifdef __cplusplus
}
#endif

#ifdef UNICODE
#define LoadLibrary		LoadLibraryW
#define LoadLibraryEx		LoadLibraryExW
#else
#define LoadLibrary		LoadLibraryA
#define LoadLibraryEx		LoadLibraryExA
#endif

#endif

#if !defined(_WIN32) && !defined(__CYGWIN__)

#ifdef __cplusplus
extern "C" {
#endif

UZI_API HMODULE GetModuleHandleA(LPCSTR lpModuleName);
UZI_API HMODULE GetModuleHandleW(LPCWSTR lpModuleName);

UZI_API DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
UZI_API DWORD GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);

UZI_API FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

UZI_API BOOL FreeLibrary(HMODULE hLibModule);

#ifdef __cplusplus
}
#endif

#ifdef UNICODE
#define GetModuleHandle		GetModuleHandleW
#define GetModuleFileName	GetModuleFileNameW
#else
#define GetModuleHandle		GetModuleHandleA
#define GetModuleFileName	GetModuleFileNameA
#endif

#endif

#endif /* UZI_LIBRARY_H */

