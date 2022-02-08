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
#define _LIB_LIBTINYRAD_LDEBUG_C 1
#include "ldebug.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#undef NDEBUG

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <syslog.h>
#include <assert.h>

#include "ldict.h"


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

// yes, global variables are evil...
const char *   tinyrad_debug_ident     = TRAD_DFLT_DEBUG_IDENT;
char           tinyrad_debug_ident_buff[128];
int            tinyrad_debug_level     = TRAD_DFLT_DEBUG_LEVEL;
int            tinyrad_debug_syslog    = TRAD_DFLT_DEBUG_SYSLOG;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

void *
tinyrad_assertions( void )
{
   static union
   {
      TinyRad                 tr;
      TinyRadDict             dict;
      TinyRadObj              obj;
      TinyRadDictAttr         attr;
      TinyRadDictAttrDef      attr_def;
      TinyRadDictValue        value;
      TinyRadDictValueDef     value_def;
      TinyRadDictVendor       vendor;
      TinyRadDictVendorDef    vendor_def;
      TinyRadMap              map;
      TinyRadURLDesc          urldesc;
   }                          data;

   // test offset of TinyRadObj
   assert( ((void *)&data.tr)       != ((void *)&data.tr.obj)     );
   assert( ((void *)&data.dict)     != ((void *)&data.dict.obj)   );
   assert( ((void *)&data.attr)     != ((void *)&data.attr.obj)   );
   assert( ((void *)&data.value)    != ((void *)&data.value.obj)  );
   assert( ((void *)&data.vendor)   != ((void *)&data.vendor.obj) );

   memset(&data, 0, sizeof(data));

   return(&data);
}


void
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


/* end of source */
