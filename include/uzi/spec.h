/**
 * WinPR: Windows Portable Runtime
 * Compiler Specification Strings
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

#ifndef UZI_SPEC_H
#define UZI_SPEC_H

#include <uzi/platform.h>

#ifdef _WIN32

#include <specstrings.h>
#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif

#else

#define DUMMYUNIONNAME u
#define DUMMYUNIONNAME1	u1
#define DUMMYUNIONNAME2	u2
#define DUMMYUNIONNAME3	u3
#define DUMMYUNIONNAME4	u4
#define DUMMYUNIONNAME5	u5
#define DUMMYUNIONNAME6	u6
#define DUMMYUNIONNAME7	u7
#define DUMMYUNIONNAME8	u8

#define DUMMYSTRUCTNAME s
#define DUMMYSTRUCTNAME1 s1
#define DUMMYSTRUCTNAME2 s2
#define DUMMYSTRUCTNAME3 s3
#define DUMMYSTRUCTNAME4 s4
#define DUMMYSTRUCTNAME5 s5

#if (defined(_M_AMD64) || defined(_M_ARM)) && !defined(_WIN32)
#define _UNALIGNED __unaligned
#else
#define _UNALIGNED
#endif

#ifndef DECLSPEC_ALIGN
#if defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#define DECLSPEC_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__)
#define DECLSPEC_ALIGN(x) __attribute__ ((__aligned__ (x)))
#else
#define DECLSPEC_ALIGN(x)
#endif
#endif /* DECLSPEC_ALIGN */

#ifdef _M_AMD64
#define MEMORY_ALLOCATION_ALIGNMENT 16
#else
#define MEMORY_ALLOCATION_ALIGNMENT 8
#endif

#ifdef __GNUC__
#ifndef __declspec
#define __declspec(e) __attribute__((e))
#endif
#endif

#ifndef DECLSPEC_NORETURN
#if (defined(__GNUC__) || defined(_MSC_VER) || defined(__clang__))
#define DECLSPEC_NORETURN __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif /* DECLSPEC_NORETURN */

#ifndef _countof
#ifndef __cplusplus
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
extern "C++" {
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper(_CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
}
#endif
#endif

/**
 * RTL Definitions
 */

#define MINCHAR		0x80
#define MAXCHAR		0x7F

#ifndef MINSHORT
#define MINSHORT	0x8000
#endif

#ifndef MAXSHORT
#define MAXSHORT	0x7FFF
#endif

#define MINLONG		0x80000000
#define MAXLONG		0x7FFFFFFF
#define MAXBYTE		0xFF
#define MAXWORD		0xFFFF
#define MAXDWORD	0xFFFFFFFF

#define FIELD_OFFSET(type, field) ((LONG)(LONG_PTR)&(((type *)0)->field))

#define RTL_FIELD_SIZE(type, field) (sizeof(((type *)0)->field))

#define RTL_SIZEOF_THROUGH_FIELD(type, field) \
	(FIELD_OFFSET(type, field) + RTL_FIELD_SIZE(type, field))

#define RTL_CONTAINS_FIELD(Struct, Size, Field) \
	((((PCHAR)(&(Struct)->Field)) + sizeof((Struct)->Field)) <= (((PCHAR)(Struct))+(Size)))

#define RTL_NUMBER_OF_V1(A) (sizeof(A) / sizeof((A)[0]))
#define RTL_NUMBER_OF_V2(A) RTL_NUMBER_OF_V1(A)

#define RTL_NUMBER_OF(A) RTL_NUMBER_OF_V1(A)

#define ARRAYSIZE(A) RTL_NUMBER_OF_V2(A)
#define _ARRAYSIZE(A) RTL_NUMBER_OF_V1(A)

#define RTL_FIELD_TYPE(type, field) (((type*)0)->field)

#define RTL_NUMBER_OF_FIELD(type, field) (RTL_NUMBER_OF(RTL_FIELD_TYPE(type, field)))

#define RTL_PADDING_BETWEEN_FIELDS(T, F1, F2) \
	((FIELD_OFFSET(T, F2) > FIELD_OFFSET(T, F1)) \
	? (FIELD_OFFSET(T, F2) - FIELD_OFFSET(T, F1) - RTL_FIELD_SIZE(T, F1)) \
	: (FIELD_OFFSET(T, F1) - FIELD_OFFSET(T, F2) - RTL_FIELD_SIZE(T, F2)))

#if defined(__cplusplus)
#define RTL_CONST_CAST(type) const_cast<type>
#else
#define RTL_CONST_CAST(type) (type)
#endif

#define RTL_BITS_OF(sizeOfArg) (sizeof(sizeOfArg) * 8)

#define RTL_BITS_OF_FIELD(type, field) (RTL_BITS_OF(RTL_FIELD_TYPE(type, field)))

#define CONTAINING_RECORD(address, type, field) \
	((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))

#endif

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef __GNUC__
    #define DECLSPEC_EXPORT __attribute__((dllexport))
    #define DECLSPEC_IMPORT __attribute__((dllimport))
  #else
    #define DECLSPEC_EXPORT __declspec(dllexport)
    #define DECLSPEC_IMPORT __declspec(dllimport)
  #endif
#else
  #if defined(__GNUC__) && __GNUC__ >= 4
    #define DECLSPEC_EXPORT __attribute__ ((visibility("default")))
    #define DECLSPEC_IMPORT
  #else
    #define DECLSPEC_EXPORT
    #define DECLSPEC_IMPORT
  #endif
#endif

#endif /* UZI_SPEC_H */

