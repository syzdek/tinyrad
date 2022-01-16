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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <syslog.h>
#include <stdarg.h>

#include <tinyrad.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#pragma mark - Macros

#undef Debug
#ifdef USE_DEBUG
#   define TinyRadDebug( level, fmt, ... ) tinyrad_debug( level, fmt, __VA_ARGS__ )
#   define TinyRadDebugTrace() tinyrad_debug( TRAD_DEBUG_TRACE, "%s()", __FUNCTION__ )
#else
#   define TinyRadDebug( level, fmt, ... ) ((void)0)
#   define TinyRadDebugTrace() ((void)0)
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

struct _tinyrad
{
   TinyRadDict *         dict;
   TinyRadURLDesc *      trud;
   TinyRadURLDesc *      trud_cur;
   size_t                trud_pos;
   struct sockaddr_in *  bind_sa;
   struct sockaddr_in6 * bind_sa6;
   struct timeval *      net_timeout;
   uint32_t              authenticator;
   uint32_t              pad32;
   unsigned              scheme;
   unsigned              opts;
   int                   s;
   int                   timeout;
};


struct _tinyrad_avp
{
   uint64_t              opts;
};


struct _tinyrad_map
{
   const char *          name;
   uint64_t              value;
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

inline void
tinyrad_debug(
         int                           level,
         const char *                  fmt,
         ... );


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#pragma mark - Inline Functions

inline void
tinyrad_debug(
         int                           level,
         const char *                  fmt,
         ... )
{
   va_list  args;

   if ( ((level & tinyrad_debug_level) == 0) || (!(fmt)) )
      return;

   if (!(tinyrad_debug_syslog))
      printf("%s: DEBUG: ", tinyrad_debug_ident);

   va_start(args, fmt);
   if ((tinyrad_debug_syslog))
      vsyslog(LOG_DEBUG, fmt, args);
   else
      vprintf(fmt, args);
   va_end(args);

   if (!(tinyrad_debug_syslog))
      printf("\n");

   return;
}

#endif /* end of header */
