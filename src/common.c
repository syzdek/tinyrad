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
#define _SRC_UTILITIES_TINYRAD_URL_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "common.h"


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

void
our_error(
         const char *                  prog_name,
         char **                       errs,
         const char *                  fmt,
         ... )
{
   int     pos;
   va_list args;

   if ((errs))
      for(pos = 0; ((errs[pos])); pos++)
         fprintf(stderr, "%s: %s\n", prog_name, errs[pos]);

   if (!(fmt))
      return;

   fprintf(stderr, "%s: ", prog_name);
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   fprintf(stderr, "\n");

   return;
}


int
our_exit_code(
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


int
our_strings_append(
         char ***                      strsp,
         const char *                  str )
{
   size_t     count;
   char **    strs;

   assert(strsp != NULL);
   assert(str   != NULL);

   // initialize array
   if (!(*strsp))
   {
      if ((*strsp = malloc(sizeof(char *))) == NULL)
         return(TRAD_ENOMEM);
      (*strsp)[0] = NULL;
   };

   // count elements
   for(count = 0; (((*strsp)[count])); count++);

   // increase size of array
   if ((strs = realloc(*strsp, (sizeof(char *)*(count+2)))) == NULL)
      return(TRAD_ENOMEM);
   *strsp = strs;

   // copy string
   if ((strs[count] = strdup(str)) == NULL)
      return(TRAD_ENOMEM);

   // terminate array
   count++;
   strs[count] = NULL;

   return(TRAD_SUCCESS);
}


void
our_strings_free(
         char **                       strs )
{
   int i;
   if (!(strs))
      return;
   for(i = 0; ((strs[i])); i++)
   {
      free(strs[i]);
      strs[i] = NULL;
   };
   free(strs);
   return;
}


void
our_version(
         const char *                  prog_name )
{
   printf("%s (%s) %s\n", prog_name, PACKAGE_NAME, PACKAGE_VERSION);
   return;
}

/* end of source */
