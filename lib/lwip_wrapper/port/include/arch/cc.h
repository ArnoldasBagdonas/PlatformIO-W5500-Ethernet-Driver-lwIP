/**
 * @file
 * @brief lwIP platform-specific definitions and compiler abstractions for SAM.
 *
 * Provides typedefs for fixed-width integers, format specifiers,
 * packing macros for different compilers, and debug/assert facilities.
 *
 * Includes Atmel ASF license terms.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef CC_H_INCLUDED
#define CC_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "arch/sys_arch.h"

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* Types based on stdint.h */
typedef uint8_t            u8_t;
typedef int8_t             s8_t;
typedef uint16_t           u16_t;
typedef int16_t            s16_t;
typedef uint32_t           u32_t;
typedef int32_t            s32_t;
typedef uintptr_t          mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* Compiler hints for packing lwip's structures */
#if defined(__CC_ARM)
    /* Setup PACKing macros for MDK Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__ICCARM__)
    /* Setup PACKing macros for EWARM Tools */
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__GNUC__)
    /* Setup PACKing macros for GCC Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#else
#error "This compiler does not support."
#endif

/* define LWIP_COMPAT_MUTEX
    to let sys.h use binary semaphores instead of mutexes - as before in 1.3.2
    Refer CHANGELOG
*/
#define  LWIP_COMPAT_MUTEX  1

/* Make lwip/arch.h define the codes which are used throughout */
#define LWIP_PROVIDE_ERRNO

/* Debug facilities. LWIP_DEBUG must be defined to read output */
#ifdef LWIP_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print a debug message.
 *
 * Prints a null-terminated string to the debug output.
 *
 * @param msg The message string to print.
 */
void lwip_debug_print(const char *msg);

/**
 * @brief Print a formatted debug message.
 *
 * Prints a formatted string to the debug output, similar to printf.
 *
 * @param fmt Format string (printf-style).
 * @param ... Variable arguments corresponding to format specifiers.
 */
void lwip_debug_printf(const char *fmt, ...);

/**
 * @brief Handle assertion failures.
 *
 * Prints an assertion failure message along with file name and line number,
 * then halts program execution.
 *
 * @param msg The assertion message.
 * @param file The source file where the assertion failed.
 * @param line The line number in the source file.
 */
void lwip_assert(const char *msg, const char *file, int line);

/**
 * @brief Dump raw data in hexadecimal format for debugging.
 *
 * @param label Label to print before dump.
 * @param data Pointer to data buffer.
 * @param len Length of data.
 */
void hex_dump_lwip(const char *label, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#define LWIP_PLATFORM_DIAG(x)        do { lwip_debug_printf x; } while(0)
#define LWIP_PLATFORM_ASSERT(x)      lwip_assert(x, __FILE__, __LINE__)

#else
#define LWIP_PLATFORM_DIAG(x)   {;}
#define LWIP_PLATFORM_ASSERT(x) {while (1);}
#endif

#endif /* CC_H_INCLUDED */
