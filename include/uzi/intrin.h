/**
 * WinPR: Windows Portable Runtime
 * C Run-Time Library Routines
 *
 * Copyright 2015 Thincast Technologies GmbH
 * Copyright 2015 Bernhard Miklautz <bernhard.miklautz@thincast.com>
 * 
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

#ifndef UZI_INTRIN_H
#define UZI_INTRIN_H

#include <uzi/uzi.h>

#ifndef _WIN32

/**
 * __lzcnt16, __lzcnt, __lzcnt64:
 * http://msdn.microsoft.com/en-us/library/bb384809/
 *
 * Beware: the result of __builtin_clz(0) is undefined
 */

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2))

static inline uint32_t __lzcnt(uint32_t _val32) {
	return ((uint32_t) __builtin_clz(_val32));
}

static inline uint16_t __lzcnt16(uint16_t _val16) {
	return ((uint16_t) (__builtin_clz((uint32_t) _val16) - 16));
}

#else /* (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2) */

static inline uint32_t __lzcnt(uint32_t x)
{
	unsigned y;
	int n = 32;
	y = x >> 16;  if (y != 0) { n = n - 16; x = y; }
	y = x >>  8;  if (y != 0) { n = n -  8; x = y; }
	y = x >>  4;  if (y != 0) { n = n -  4; x = y; }
	y = x >>  2;  if (y != 0) { n = n -  2; x = y; }
	y = x >>  1;  if (y != 0) return n - 2;
	return n - x;
}

static inline uint16_t __lzcnt16(uint16_t x)
{
	return ((uint16_t) __lzcnt((uint16_t) x));
}

#endif /* (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2) */

#endif /* _WIN32 */
#endif /* UZI_INTRIN_H */
