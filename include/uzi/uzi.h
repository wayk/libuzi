
#ifndef UZI_API_H
#define UZI_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* UZI_HANDLE;

#define UZI_MAX_HANDLES			64
#define UZI_INFINITE			0xFFFFFFFF
#define UZI_WAIT_OBJECT_0		0x00000000
#define UZI_WAIT_ABANDONED		0x00000080
#define UZI_WAIT_TIMEOUT		0x00000102
#define UZI_WAIT_FAILED			0xFFFFFFFF

UZI_HANDLE UziCreateEvent(bool manualReset, bool initialState);
bool UziSetEvent(UZI_HANDLE handle);
bool UziResetEvent(UZI_HANDLE handle);
bool UziEventIsSet(UZI_HANDLE handle);
bool UziCloseHandle(UZI_HANDLE handle);

uint32_t UziWaitSingle(UZI_HANDLE handle, uint32_t timeout);
uint32_t UziWaitMulti(uint32_t nCount, const UZI_HANDLE* handles, bool waitAll, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* UZI_API_H */

