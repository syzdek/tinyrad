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
#include <stdatomic.h>
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
   TinyRad                 tr;
   TinyRadDict             dict;
   TinyRadDictAttr         attr;
   //TinyRadDictAttrDef      attrdef;
   TinyRadDictValue        value;
   //TinyRadDictValueDef     valuedef;
   TinyRadDictVendor       vendor;
   //TinyRadDictVendorDef    vendordef;
   //TinyRadMap              map;
   TinyRadObj              obj;
   //TinyRadURLDesc          urldesc;

   // test size of structs
   assert( sizeof(TinyRad)             > sizeof(TinyRadObj) );
   assert( sizeof(TinyRadDict)         > sizeof(TinyRadObj) );
   assert( sizeof(TinyRadDictAttr)     > sizeof(TinyRadObj) );
   assert( sizeof(TinyRadDictValue)    > sizeof(TinyRadObj) );
   assert( sizeof(TinyRadDictVendor)   > sizeof(TinyRadObj) );

   // test offset of TinyRadObj
   assert( ((void *)&tr)       == ((void *)&tr.obj)     );
   assert( ((void *)&dict)     == ((void *)&dict.obj)   );
   assert( ((void *)&attr)     == ((void *)&attr.obj)   );
   assert( ((void *)&value)    == ((void *)&value.obj)  );
   assert( ((void *)&vendor)   == ((void *)&vendor.obj) );

   // initialize variables
   memset(&tr,       0, sizeof(tr));
   memset(&dict,     0, sizeof(dict));
   memset(&attr,     0, sizeof(attr));
   memset(&value,    0, sizeof(value));
   memset(&vendor,   0, sizeof(vendor));
   memset(&obj,      0, sizeof(obj));

   // set magic numbers
   memcpy(&tr,       TRAD_MAGIC, 8);
   memcpy(&dict,     TRAD_MAGIC, 8);
   memcpy(&attr,     TRAD_MAGIC, 8);
   memcpy(&value,    TRAD_MAGIC, 8);
   memcpy(&vendor,   TRAD_MAGIC, 8);
   memcpy(&obj,      TRAD_MAGIC, 8);

   // initialize ref_count
   atomic_init( &((TinyRadObj *)&tr)->ref_count,      0xf0f00f0f );
   atomic_init( &((TinyRadObj *)&dict)->ref_count,    0xf0f00f0f );
   atomic_init( &((TinyRadObj *)&attr)->ref_count,    0xf0f00f0f );
   atomic_init( &((TinyRadObj *)&value)->ref_count,   0xf0f00f0f );
   atomic_init( &((TinyRadObj *)&vendor)->ref_count,  0xf0f00f0f );
   atomic_init( &((TinyRadObj *)&obj)->ref_count,     0xf0f00f0f );

   // verify magic numbers
   assert( memcmp(tr.obj.magic,       TRAD_MAGIC, 8) == 0 );
   assert( memcmp(dict.obj.magic,     TRAD_MAGIC, 8) == 0 );
   assert( memcmp(attr.obj.magic,     TRAD_MAGIC, 8) == 0 );
   assert( memcmp(value.obj.magic,    TRAD_MAGIC, 8) == 0 );
   assert( memcmp(vendor.obj.magic,   TRAD_MAGIC, 8) == 0 );
   assert( memcmp(obj.magic,          TRAD_MAGIC, 8) == 0 );

   // verify ref_count
   assert( (unsigned long)atomic_fetch_add( &tr.obj.ref_count,       0 ) == 0xf0f00f0fUL );
   assert( (unsigned long)atomic_fetch_add( &dict.obj.ref_count,     0 ) == 0xf0f00f0fUL );
   assert( (unsigned long)atomic_fetch_add( &attr.obj.ref_count,     0 ) == 0xf0f00f0fUL );
   assert( (unsigned long)atomic_fetch_add( &value.obj.ref_count,    0 ) == 0xf0f00f0fUL );
   assert( (unsigned long)atomic_fetch_add( &vendor.obj.ref_count,   0 ) == 0xf0f00f0fUL );
   assert( (unsigned long)atomic_fetch_add( &obj.ref_count,          0 ) == 0xf0f00f0fUL );

   return(NULL);
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
