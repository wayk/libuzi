/**
 * WinPR: Windows Portable Runtime
 * Error Handling Functions
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

#ifndef UZI_ERROR_H
#define UZI_ERROR_H

#include <uzi/winpr.h>
#include <uzi/wtypes.h>

#ifdef _WIN32

#include <winerror.h>

#else

#define WAIT_TIMEOUT									0x00000102

#define ERROR_INVALID_HANDLE								0x00000006
#define ERROR_INVALID_PARAMETER								0x00000057
#define ERROR_INTERNAL_ERROR								0x0000054F

#define WSAEINTR									0x00002714
#define WSAEBADF									0x00002719
#define WSAEACCES									0x0000271D
#define WSAEFAULT									0x0000271E
#define WSAEINVAL									0x00002726
#define WSAEMFILE									0x00002728
#define WSAEWOULDBLOCK									0x00002733
#define WSAEINPROGRESS									0x00002734
#define WSAEALREADY									0x00002735
#define WSAENOTSOCK									0x00002736
#define WSAEDESTADDRREQ									0x00002737
#define WSAEMSGSIZE									0x00002738
#define WSAEPROTOTYPE									0x00002739
#define WSAENOPROTOOPT									0x0000273A
#define WSAEPROTONOSUPPORT								0x0000273B
#define WSAESOCKTNOSUPPORT								0x0000273C
#define WSAEOPNOTSUPP									0x0000273D
#define WSAEPFNOSUPPORT									0x0000273E
#define WSAEAFNOSUPPORT									0x0000273F
#define WSAEADDRINUSE									0x00002740
#define WSAEADDRNOTAVAIL								0x00002741
#define WSAENETDOWN									0x00002742
#define WSAENETUNREACH									0x00002743
#define WSAENETRESET									0x00002744
#define WSAECONNABORTED									0x00002745
#define WSAECONNRESET									0x00002746
#define WSAENOBUFS									0x00002747
#define WSAEISCONN									0x00002748
#define WSAENOTCONN									0x00002749
#define WSAESHUTDOWN									0x0000274A
#define WSAETOOMANYREFS									0x0000274B
#define WSAETIMEDOUT									0x0000274C
#define WSAECONNREFUSED									0x0000274D
#define WSAELOOP									0x0000274E
#define WSAENAMETOOLONG									0x0000274F
#define WSAEHOSTDOWN									0x00002750
#define WSAEHOSTUNREACH									0x00002751
#define WSAENOTEMPTY									0x00002752
#define WSAEPROCLIM									0x00002753
#define WSAEUSERS									0x00002754
#define WSAEDQUOT									0x00002755
#define WSAESTALE									0x00002756
#define WSAEREMOTE									0x00002757

#ifdef __cplusplus
extern "C" {
#endif

UZI_API DWORD GetLastError(void);

UZI_API VOID SetLastError(DWORD dwErrCode);

#ifdef __cplusplus
}
#endif

#endif

#endif /* UZI_ERROR_H */

