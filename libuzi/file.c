
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <io.h>
#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib, "shlwapi.lib")
#endif

#include <uzi/uzi.h>

int UziFile_Seek(FILE* fp, uint64_t offset, int origin)
{
#ifdef _WIN32
	return (int) _fseeki64(fp, offset, origin);
#elif defined(__APPLE__)
	return (int) fseeko(fp, offset, origin);
#else
	return (int) fseeko(fp, offset, origin);
#endif
}

uint64_t UziFile_Tell(FILE* fp)
{
#ifdef _WIN32
	return (uint64_t) _ftelli64(fp);
#elif defined(__APPLE__)
	return (uint64_t) ftello(fp);
#else
	return (uint64_t) ftello(fp);
#endif
}

uint64_t UziFile_Size(const char* filename)
{
	FILE* fp;
	uint64_t fileSize;

	fp = fopen(filename, "r");

	if (!fp)
		return 0;

	UziFile_Seek(fp, 0, SEEK_END);
	fileSize = UziFile_Tell(fp);
	fclose(fp);

	return fileSize;
}

const char* UziFile_Base(const char* filename)
{
	size_t length;
	char* separator;

	if (!filename)
		return NULL;

	separator = strrchr(filename, '\\');

	if (!separator)
		separator = strrchr(filename, '/');

	if (!separator)
		return filename;

	length = strlen(filename);

	if ((length - (separator - filename)) > 1)
		return separator + 1;

	return filename;
}

char* UziFile_Dir(const char* filename)
{
	char* dir;
	char* end;
	char* base;
	size_t length;

	base = (char*) UziFile_Base(filename);

	if (!base)
		return NULL;

	end = base - 1;

	if (end < filename)
		return NULL;

	length = end - filename;
	dir = malloc(length + 1);

	if (!dir)
		return NULL;

	memcpy(dir, filename, length);
	dir[length] = '\0';

	return dir;
}

const char* UziFile_Extension(const char* filename, bool dot)
{
	char* p;
	size_t length;

	if (!filename)
		return NULL;

	p = strrchr(filename, '.');

	if (!p)
		return NULL;

	if (dot)
		return p;

	length = strlen(filename);

	if ((length - (p - filename)) > 1)
		return p + 1;

	return NULL;
}

FILE* UziFile_Open(const char* path, const char* mode)
{
#ifndef _WIN32
	return fopen(path, mode);
#else
	FILE* fp = NULL;
	uint16_t modeW[32];
	uint16_t pathW[UZI_MAX_PATH];

	if (!path || !mode)
		return NULL;

	UziUtf8toUtf16((uint8_t*) mode, -1, modeW, sizeof(modeW) / 2);
	UziUtf8toUtf16((uint8_t*) path, -1, pathW, sizeof(pathW) / 2);

	fp = _wfopen(pathW, modeW);

	return fp;
#endif
}

uint8_t* UziFile_Load(const char* filename, size_t* size, uint32_t padding)
{
	FILE* fp = NULL;
	uint8_t* data = NULL;

	if (!filename || !size)
		return NULL;

	*size = 0;

	fp = UziFile_Open(filename, "rb");

	if (!fp)
		return NULL;

	UziFile_Seek(fp, 0, SEEK_END);
	*size = UziFile_Tell(fp);
	UziFile_Seek(fp, 0, SEEK_SET);

	data = malloc(*size + padding);

	if (!data)
		goto exit;

	memset(&data[*size], 0, padding);

	if (fread(data, 1, *size, fp) != *size)
	{
		free(data);
		data = NULL;
		*size = 0;
	}

	if (padding)
		*size += padding;

exit:
	fclose(fp);
	return data;
}

bool UziFile_Save(const char* filename, uint8_t* data, size_t size, int mode)
{
	FILE* fp = NULL;
	bool success = true;

	if (!filename || !data)
		return false;

	fp = UziFile_Open(filename, "wb");

	if (!fp)
		return false;

	if (fwrite(data, 1, size, fp) != size)
	{
		success = false;
	}

	fclose(fp);
	return success;
}

bool UziFile_Move(const char* src, const char* dst, bool replace)
{
#ifdef _WIN32
	uint16_t srcW[UZI_MAX_PATH];
	uint16_t dstW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) src, -1, srcW, sizeof(srcW) / 2);
	UziUtf8toUtf16((uint8_t*) dst, -1, dstW, sizeof(dstW) / 2);

	return MoveFileW(srcW, dstW) ? true : false;
#else
	int status;
	struct stat st;

	status = stat(dst, &st);

	if (!replace)
	{
		if (status == 0)
			return false; /* ERROR_ALREADY_EXISTS */
	}
	else
	{
		if ((status == 0) && ((st.st_mode & S_IWUSR) == 0))
			return false; /* ERROR_ACCESS_DENIED */
	}

	status = rename(src, dst);

	return (status == 0) ? true : false;
#endif
}

bool UziFile_Exists(const char* filename)
{
#ifdef _WIN32
	uint16_t filenameW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) filename, -1, filenameW, sizeof(filenameW) / 2);

	return PathFileExistsW(filenameW) ? true : false;
#else
	struct stat stat_info;

	if (stat(filename, &stat_info) != 0)
		return false;

	return true;
#endif
}

bool UziFile_Delete(const char* filename)
{
#ifdef _WIN32
	uint16_t filenameW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) filename, -1, filenameW, sizeof(filenameW) / 2);

	return DeleteFileW(filenameW) ? true : false;
#else
	int status;
	status = unlink(filename);
	return (status != -1) ? true : false;
#endif
}

int UziMkPath(const char* path, int mode)
{
	char tmp;
	char *p, *q;
	int status = 0;

	p = (char*) path;
	q = strchr(p, UZI_PATH_SEPARATOR_CHR);

	while (p || q)
	{
		if (q)
		{
			tmp = *q;
			*q = '\0';
		}

		status = UziMkDir(path, mode);

		if (q)
			*q = tmp;

		if (!q)
			break;

		p = q + 1;
		q = strchr(p, UZI_PATH_SEPARATOR_CHR);
	}

	return status;
}

int UziMkDir(const char* path, int mode)
{
	int status;

#ifdef _WIN32
	uint16_t pathW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) path, -1, pathW, sizeof(pathW) / 2);

	status = CreateDirectoryW(pathW, NULL) ? 0 : -1;
#else
	if (!mode)
		mode = S_IRUSR | S_IWUSR | S_IXUSR;

	status = mkdir(path, mode);
#endif

	return status;
}

int UziRmDir(const char* path)
{
	int status;

#ifdef _WIN32
	uint16_t pathW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) path, -1, pathW, sizeof(pathW) / 2);

	status = RemoveDirectoryW(pathW) ? 0 : -1;
#else
	status = rmdir(path);
#endif

	return status;
}

int UziChMod(const char* filename, int mode)
{
#ifdef _WIN32
	int status;
	uint16_t filenameW[UZI_MAX_PATH];

	UziUtf8toUtf16((uint8_t*) filename, -1, filenameW, sizeof(filenameW) / 2);

	status = _wchmod(filenameW, mode);

	return status;
#else
	mode_t fl = 0;

	fl |= (mode & 0x4000) ? S_ISUID : 0;
	fl |= (mode & 0x2000) ? S_ISGID : 0;
	fl |= (mode & 0x1000) ? S_ISVTX : 0;
	fl |= (mode & 0x0400) ? S_IRUSR : 0;
	fl |= (mode & 0x0200) ? S_IWUSR : 0;
	fl |= (mode & 0x0100) ? S_IXUSR : 0;
	fl |= (mode & 0x0040) ? S_IRGRP : 0;
	fl |= (mode & 0x0020) ? S_IWGRP : 0;
	fl |= (mode & 0x0010) ? S_IXGRP : 0;
	fl |= (mode & 0x0004) ? S_IROTH : 0;
	fl |= (mode & 0x0002) ? S_IWOTH : 0;
	fl |= (mode & 0x0001) ? S_IXOTH : 0;

	return chmod(filename, fl);
#endif
}
