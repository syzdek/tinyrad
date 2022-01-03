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
#define _LIB_LIBTINYRAD_LMEMORY_C 1
#include "lmemory.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

/// destroy Tiny RADIUS reference
///
/// @param[in]  tr            dictionary reference
void
tinyrad_destroy(
         TinyRad *                     tr )
{
   assert(tr != NULL);

   bzero(tr, sizeof(TinyRad));
   free(tr);

   return;
}


void
tinyrad_free(
         void *                        ptr )
{
   free(ptr);
   return;
}


/// initialize Tiny RADIUS reference
///
/// @param[out] trp           pointer to Tiny RADIUS reference
/// @param[in]  server        RADIUS server
/// @param[in]  opts          initialization options
/// @return returns error code
int
tinyrad_initialize(
         TinyRad **                    trp,
         const char *                  server,
         uint64_t                      opts )
{
   TinyRad   * tr;

   assert(trp    != NULL);
   assert(server != NULL);

   if ((tr = malloc(sizeof(TinyRad))) == NULL)
      return(TRAD_ENOMEM);
   bzero(tr, sizeof(TinyRad));
   tr->opts = (uint32_t)opts;

   *trp = tr;

   return(TRAD_SUCCESS);
}


/* end of source */
