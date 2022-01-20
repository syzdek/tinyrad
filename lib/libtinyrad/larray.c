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
#define _LIB_LIBTINYRAD_LARRAY_C 1
#include "larray.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

void
tinyrad_array_move(
         void *                        a,
         void *                        b,
         size_t                        size );


void
tinyrad_array_swap(
         void *                        a,
         void *                        b,
         size_t                        size );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

void *
tinyrad_array_get(
         void *                        base,
         size_t                        nel,
         size_t                        width,
         const void *                  key,
         int (*compar)(const void *, const void *) )
{
   ssize_t     idx;
   TinyRadDebugTrace();
   assert(base  != NULL);
   assert(key   != NULL);
   assert(width  > 0);
   if ((idx = tinyrad_array_search(base, nel, width, key, NULL, compar)) == -1)
      return(NULL);
   return(((char *)base) + (width * (size_t)idx));
}


ssize_t
tinyrad_array_insert(
         void *                        base,
         size_t                        nel,
         size_t                        width,
         void *                        obj,
         int                           action,
         int  (*compar)(const void *, const void *),
         void (*freeobj)(void *) )
{
   ssize_t     idx;
   size_t      wouldbe;
   ssize_t     pos;
   char *      src;
   char *      dst;

   TinyRadDebugTrace();

   assert(base  != NULL);
   assert(obj   != NULL);
   assert(width  > 0);

   if (!(nel))
   {
      tinyrad_array_move(obj, base, width);
      return(0);
   };

   // search for existing object which matches
   if ((idx = tinyrad_array_search(base, nel, width, obj, &wouldbe, compar)) != -1)
   {
      if (action == TINYRAD_ARRAY_INSERT)
         return(-1);
      if (action == TINYRAD_ARRAY_REPLACE)
      {
         dst = ((char *)base) + (width * (size_t)idx);
         if ((freeobj))
            (*freeobj)(dst);
         tinyrad_array_move(obj, dst, width);
         return(idx);
      };
      if (action != TINYRAD_ARRAY_APPEND)
         return(-1);
   };

   // shift list
   for(pos = (ssize_t)(nel-1); (pos >= (ssize_t)wouldbe); pos--)
   {
      src = ((char *)base) + (width * (size_t)(pos+0));
      dst = ((char *)base) + (width * (size_t)(pos+1));
      tinyrad_array_move(src, dst, width);
   };

   // save object
   dst = ((char *)base) + (width * wouldbe);
   tinyrad_array_move(obj, dst, width);

   return((ssize_t)wouldbe);
}


void
tinyrad_array_move(
         void *                        a,
         void *                        b,
         size_t                        size )
{
   uint8_t *   x;
   uint8_t *   y;
   size_t      s;
   TinyRadDebugTrace();
   x = a;
   y = b;
   for(s = size; (s > 0); s--)
      *y++ = *x++;
   return;
}


ssize_t
tinyrad_array_remove(
         void *                        base,
         size_t                        nel,
         size_t                        width,
         const void *                  key,
         int (*compar)(const void *, const void *),
         void (*freeobj)(void *) )
{
   ssize_t     idx;
   char *      src;
   char *      dst;
   size_t      pos;

   TinyRadDebugTrace();

   assert(base  != NULL);
   assert(key   != NULL);
   assert(width  > 0);

   // search for matching object
   if ((idx = tinyrad_array_search(base, nel, width, key, NULL, compar)) == -1)
      return(-1);

   // free object
   if ((freeobj))
   {
      dst = ((char *)base) + (width * (size_t)idx);
      (*freeobj)(dst);
   };

   // shift list
   for(pos = ((size_t)idx); (pos < (nel-1)); pos ++)
   {
      src = ((char *)base) + (width * (size_t)(pos+1));
      dst = ((char *)base) + (width * (size_t)(pos+0));
      tinyrad_array_move(src, dst, width);
   };

   return(0);
}


ssize_t
tinyrad_array_search(
         const void *                  base,
         size_t                        nel,
         size_t                        width,
         const void *                  key,
         size_t *                      wouldbep,
         int (*compar)(const void *, const void *) )
{
   size_t         wouldbe;
   ssize_t        low;
   ssize_t        mid;
   ssize_t        high;
   int            rc;
   const char *   ptr;

   TinyRadDebugTrace();

   assert(base  != NULL);
   assert(key   != NULL);
   assert(width  > 0);

   if (nel == 0)
   {
      if ((wouldbep))
         *wouldbep = 0;
      return(-1);
   };

   if (!(wouldbep))
      wouldbep = &wouldbe;

   low  = 0;
   high = (ssize_t)(nel - 1);
   mid  = (ssize_t)((nel - 1) / 2);

   while ( (mid >= low) && (mid <= high) && (high != low) )
   {
      ptr = ((const char *)base) + (width * (size_t)mid);
      if ((rc = (*compar)(ptr, key)) == 0)
         high = mid;
      else if (rc > 0)
         high = mid - 1;
      else
         low = mid + 1;
      mid = (high + low) / 2;
   };

   ptr = ((const char *)base) + (width * (size_t)mid);
   if ((rc = (*compar)(ptr, key)) == 0)
      return(*((ssize_t *)wouldbep) = mid);

   *wouldbep = (size_t)((rc > 0) ? mid : mid+1);

   return(-1);
}


void
tinyrad_array_swap(
         void *                        a,
         void *                        b,
         size_t                        size )
{
   uint8_t *   x;
   uint8_t *   y;
   uint8_t     t;
   size_t      s;
   TinyRadDebugTrace();
   x = a;
   y = b;
   for(s = size; (s > 0); s--)
   {
      t    = *x;
      *x++ = *y;
      *y++ = t;
   };
   return;
}


/* end of source */