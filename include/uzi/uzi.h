
#ifndef UZI_API_H
#define UZI_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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

struct uzi_cs
{
	uint8_t opaque[32];
};
typedef struct uzi_cs UZI_CS;

bool UziInitCS(UZI_CS* cs, uint32_t spinCount, uint32_t flags);
void UziEnterCS(UZI_CS* cs);
bool UziTryEnterCS(UZI_CS* cs);
void UziLeaveCS(UZI_CS* cs);
void UziDeleteCS(UZI_CS* cs);

int UziUtf8toUtf16(const uint8_t* src, int cchSrc, uint16_t* dst, int cchDst);
int UziUtf16toUtf8(const uint16_t* src, int cchSrc, uint8_t* dst, int cchDst);

#ifdef _WIN32
#define uzi_snprintf		sprintf_s
#else
#define uzi_snprintf		snprintf
#endif

/**
 * File API
 */

#define UZI_MAX_PATH		1024

#define UZI_PATH_SLASH_CHR		'/'
#define UZI_PATH_SLASH_STR		"/"

#define UZI_PATH_BACKSLASH_CHR		'\\'
#define UZI_PATH_BACKSLASH_STR		"\\"

#ifdef _WIN32
#define UZI_PATH_SEPARATOR_CHR		UZI_PATH_BACKSLASH_CHR
#define UZI_PATH_SEPARATOR_STR		UZI_PATH_BACKSLASH_STR
#else
#define UZI_PATH_SEPARATOR_CHR		UZI_PATH_SLASH_CHR
#define UZI_PATH_SEPARATOR_STR		UZI_PATH_SLASH_STR
#endif

int UziFile_Seek(FILE* fp, uint64_t offset, int origin);
uint64_t UziFile_Tell(FILE* fp);
uint64_t UziFile_Size(const char* filename);
const char* UziFile_Base(const char* filename);
char* UziFile_Dir(const char* filename);
const char* UziFile_Extension(const char* filename, bool dot);
FILE* UziFile_Open(const char* path, const char* mode);
uint8_t* UziFile_Load(const char* filename, size_t* size, uint32_t padding);
bool UziFile_Save(const char* filename, uint8_t* data, size_t size, int mode);
bool UziFile_Move(const char* src, const char* dst, bool replace);
bool UziFile_Exists(const char* filename);
bool UziFile_Delete(const char* filename);
int UziMkPath(const char* path, int mode);
int UziMkDir(const char* path, int mode);
int UziRmDir(const char* path);
int UziChMod(const char* filename, int mode);

#ifdef __cplusplus
}
#endif

#endif /* UZI_API_H */

