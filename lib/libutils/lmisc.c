/*
 *  Tiny RADIUS Client Library
 *  Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.
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
#define _LIB_LIBTRUTILS_LMISC_C 1
#include "lmisc.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <assert.h>


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

int
trutils_exit_code(
         int                           rc )
{
   switch(rc)
   {
      case TRAD_SUCCESS:
      return(0);

      // configuration error
      case TRAD_EURL:
      case TRAD_ESYNTAX:
      return(2);

      // network error
      case TRAD_ERESOLVE:
      return(3);

      default:
      break;
   };
   return(1);
}


char *
trutils_strrand(
         size_t                        min,
         size_t                        max )
{
   char *      str;
   size_t      len;
   size_t      pos;
   static int  count = 0;

   assert(min <= max);

   len = ((max - min)) ? ((unsigned)random() % (max - min)) : 0;
   len += min;
   if (len < sizeof(int))
      len = sizeof(int);

   if ((str = malloc(len+1)) == NULL)
      return(NULL);
   str[len] = '\0';

   for(pos = 0; (pos < (len-sizeof(int))); pos++)
      str[pos] = (random() % 26) + 'a';

   snprintf(&str[pos], (len-pos+1), "%04xu", (~count)&0xffff);

   count++;

   return(str);
}


/* end of source */
