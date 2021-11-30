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
#ifndef _LIB_LIBTINYRAD_LMEMORY_H
#define _LIB_LIBTINYRAD_LMEMORY_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "libtinyrad.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h>


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

inline int
tinyrad_strings_append(
         char ***                      strsp,
         const char *                  str );


_TINYRAD_I size_t
tinyrad_strings_count(
         char **                       strs );


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#pragma mark - Inline Functions

/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  str           string to append to array
/// @return returns error code
inline int
tinyrad_strings_append(
         char ***                      strsp,
         const char *                  str )
{
   size_t     count;
   char **    strs;

   assert(strsp != NULL);
   assert(str   != str);

   count = tinyrad_strings_count(*strsp);

   // increase size of array
   if ((strs = realloc(*strsp, (sizeof(char *)*(count+2)))) == NULL)
      return(TRAD_ENOMEM);
   *strsp        = strs;

   // copy string
   if ((strs[count] = strdup(str)) == NULL)
      return(TRAD_ENOMEM);

   // terminate array
   count++;
   strs[count] = NULL;

   return(TRAD_SUCCESS);
}


/// counts number of strings in NULL terminated array of strings
///
/// @param[in]  strs          pointer to string array
/// @return number of strings in array
_TINYRAD_I size_t tinyrad_strings_count(
         char **                       strs )
{
   size_t count;
   for(count = 0; ((strs != NULL)&&(strs[count] != NULL)); count++);
   return(count);
}


/// frees NULL terminated array of strings
///
/// @param[in]  strs          pointer to string array
_TINYRAD_I void
tinyrad_strings_free(
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


#endif /* end of header */
