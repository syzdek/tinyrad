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
#define _LIB_LIBTINYRAD_LOID_C 1
#include "loid.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

//----------------------//
// TinyRadOID functions //
//----------------------//
#pragma mark TinyRadOID functions

//------------------//
// memory functions //
//------------------//
#pragma mark memory functions

TinyRadOID *
tinyrad_oid_alloc(
         void )
{
   TinyRadOID *      ptr;

   if ((ptr = malloc(sizeof(TinyRadOID))) == NULL)
      return(ptr);
   memset(ptr, 0, sizeof(TinyRadOID));

   return(ptr);
}


TinyRadOID *
tinyrad_oid_dup(
         const TinyRadOID *            ptr )
{
   TinyRadOID *         oid;
   size_t               pos;

   assert(ptr           != NULL);

   // copy verbatim
   if ((oid = malloc(sizeof(TinyRadOID))) == NULL)
      return(oid);
   memcpy(oid, ptr, sizeof(TinyRadOID));

   // zero unused values
   for(pos = oid->oid_len; (pos < TRAD_OID_MAX_LEN); pos++)
      oid->oid_val[pos] = 0;

   return(oid);
}


uint32_t
tinyrad_oid_pop(
         TinyRadOID *                  oid )
{
   assert(oid != NULL);
   if (!(oid->oid_len))
      return(0);
   oid->oid_len--;
   return(oid->oid_val[oid->oid_len]);
}


int
tinyrad_oid_push(
         TinyRadOID *                  oid,
         uint32_t                      val )
{
   assert(oid != NULL);
   assert((oid->oid_len+1) < TRAD_OID_MAX_LEN);

   oid->oid_val[oid->oid_len] = val;
   oid->oid_len++;

   return(TRAD_SUCCESS);
}


//-----------------//
// query functions //
//-----------------//
#pragma mark query functions

int
tinyrad_oid_cmp(
         const void *                 a,
         const void *                 b )
{
   const TinyRadOID * const * x = a;
   const TinyRadOID * const * y = b;
   size_t                     len;
   size_t                     pos;

   len = ((*x)->oid_len < (*y)->oid_len) ? (*x)->oid_len : (*y)->oid_len;

   for(pos = 0; (pos < len); pos++)
   {
      if ((*x)->oid_val[pos] != (*y)->oid_val[pos])
         return( ((*x)->oid_val[pos] < (*y)->oid_val[pos]) ? -1 : 1 );
   };

   if ((*x)->oid_len != (*y)->oid_len)
      return( ((*x)->oid_len < (*y)->oid_len) ? -1 : 1 );

   return(0);
}


uint32_t
tinyrad_oid_type(
         const TinyRadOID *            oid )
{
   assert(oid != NULL);
   return(oid->oid_val[0]);
}


uint32_t
tinyrad_oid_vendor_id(
         const TinyRadOID *            oid )
{
   assert(oid != NULL);
   switch(oid->oid_val[0])
   {
      case TRAD_ATTR_VENDOR_SPECIFIC:
      if (oid->oid_len < 2)
         return(0);
      return(oid->oid_val[1]);

      case TRAD_ATTR_EXTENDED_ATTRIBUTE_1:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_2:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_3:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_4:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_5:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_6:
      if (oid->oid_len < 3)
         return(0);
      if (oid->oid_val[1] != 26)
         return(0);
      return(oid->oid_val[2]);

      default:
      break;
   };
   return(0);
}


uint32_t
tinyrad_oid_vendor_type(
         const TinyRadOID *            oid )
{
   assert(oid != NULL);
   switch(oid->oid_val[0])
   {
      case TRAD_ATTR_VENDOR_SPECIFIC:
      if (oid->oid_len < 3)
         return(0);
      return(oid->oid_val[2]);

      case TRAD_ATTR_EXTENDED_ATTRIBUTE_1:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_2:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_3:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_4:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_5:
      case TRAD_ATTR_EXTENDED_ATTRIBUTE_6:
      if (oid->oid_len < 4)
         return(0);
      if (oid->oid_val[1] != 26)
         return(0);
      return(oid->oid_val[3]);

      default:
      break;
   };
   return(0);
}


/* end of source */
