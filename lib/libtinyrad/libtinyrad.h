/*
 *  Tiny RADIUS Client Library
 *  Copyright (C) 2021 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of David M. Syzdek nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */
#ifndef _LIB_LIBTINYRAD_H
#define _LIB_LIBTINYRAD_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <tinyrad_compat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdarg.h>

#include <tinyrad.h>
#include <tinyrad_internal.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#pragma mark - Macros

#undef Debug
#ifdef USE_DEBUG
#   define TinyRadDebug( level, fmt, ... ) tinyrad_debug( level, fmt, __VA_ARGS__ )
#   define TinyRadDebugTrace() tinyrad_debug( TRAD_DEBUG_TRACE, "%s()", __func__ )
#else
#   define TinyRadDebug( level, fmt, ... ) ((void)0)
#   define TinyRadDebugTrace() ((void)0)
#endif

#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "TinyRad"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION "n/a"
#endif

///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

// Magic number used to determine if memory passed to tinyrad_free() is a
// "TinyRadObj" or raw virtual memory.  The magic number purposely starts
// and ends with a byte equal to zero so that it cannot match a string.
#define TRAD_MAGIC                  ((const uint8_t *)"\0TnyRad\0")
//#define TRAD_MAGIC                  0x00546E7952616400ULL   // value of "\0TnyRad\0"


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _tinyrad_obj TinyRadObj;


struct _tinyrad_obj
{
   uint8_t                 magic[8];
   atomic_intptr_t         ref_count;
   void (*free_func)(void * ptr);
};


struct _tinyrad
{
   TinyRadObj            obj;
   TinyRadDict *         dict;
   TinyRadURLDesc *      trud;
   TinyRadURLDesc *      trud_cur;
   char *                secret;
   char *                secret_file;
   void *                padptr;
   size_t                trud_pos;
   struct sockaddr_in *  bind_sa;
   struct sockaddr_in6 * bind_sa6;
   struct timeval *      net_timeout;
   uint32_t              authenticator;
   uint32_t              scheme;
   unsigned              opts;
   unsigned              opts_neg;
   int                   s;
   int                   timeout;
   int                   rand;
   int                   padint;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

extern const char *   tinyrad_debug_ident;
extern char           tinyrad_debug_ident_buff[128];
extern int            tinyrad_debug_level;
extern int            tinyrad_debug_syslog;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

void
tinyrad_debug(
         int                           level,
         const char *                  fmt,
         ... );


#endif /* end of header */
