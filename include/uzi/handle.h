/**
 * WinPR: Windows Portable Runtime
 * Handle Management
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

#ifndef UZI_HANDLE_H
#define UZI_HANDLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uzi/uzi.h>
#include <uzi/wtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE			((HANDLE) (LONG_PTR) - 1)
#endif

#define UZI_FD_READ_BIT				0
#define UZI_FD_READ					(1 << UZI_FD_READ_BIT)

#define UZI_FD_WRITE_BIT				1
#define UZI_FD_WRITE					(1 << UZI_FD_WRITE_BIT)

#ifndef _WIN32

#define DUPLICATE_CLOSE_SOURCE			0x00000001
#define DUPLICATE_SAME_ACCESS			0x00000002

#define HANDLE_FLAG_INHERIT			0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE		0x00000002

UZI_API BOOL CloseHandle(HANDLE hObject);

UZI_API BOOL DuplicateHandle(HANDLE hSourceProcessHandle,
				HANDLE hSourceHandle,
				HANDLE hTargetProcessHandle,
				LPHANDLE lpTargetHandle,
				DWORD dwDesiredAccess,
				BOOL bInheritHandle, DWORD dwOptions);

#endif

#ifdef __cplusplus
}
#endif

#endif /* UZI_HANDLE_H */

