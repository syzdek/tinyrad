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
#ifndef _LIB_LIBTINYRAD_LSTRINGS_H
#define _LIB_LIBTINYRAD_LSTRINGS_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "libtinyrad.h"

#include <stdio.h>
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

inline size_t
tinyrad_strings_count(
         char **                       strs );


inline char *
tinyrad_strings_dequeue(
         char **                       strs,
         int                           freeit );


inline intmax_t
tinyrad_strings_dequeue_int(
         char **                       strs );


inline uintmax_t
tinyrad_strings_dequeue_uint(
         char **                       strs );


inline int
tinyrad_strings_dup(
         char **                       src,
         char ***                      destp );


inline int
tinyrad_strings_enqueue(
         char ***                      strsp,
         const char *                  str );


inline int
tinyrad_strings_enqueue_int(
         char ***                      strsp,
         intmax_t                      i );


inline int
tinyrad_strings_enqueue_uint(
         char ***                      strsp,
         uintmax_t                     uint );


inline char *
tinyrad_strings_pop(
         char **                       strs,
         int                           freeit );


inline intmax_t
tinyrad_strings_pop_int(
         char **                       strs );


inline uintmax_t
tinyrad_strings_pop_uint(
         char **                       strs );


inline int
tinyrad_strings_push(
         char ***                      strsp,
         const char *                  str );


inline int
tinyrad_strings_push_int(
         char ***                      strsp,
         intmax_t                      i );


inline int
tinyrad_strings_push_uint(
         char ***                      strsp,
         uintmax_t                     uint );


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#pragma mark - Inline Functions

/// counts number of strings in NULL terminated array of strings
///
/// @param[in]  strs          pointer to string array
/// @return number of strings in array
inline size_t
tinyrad_strings_count(
         char **                       strs )
{
   size_t count;
   TinyRadDebugTrace();
   if (!(strs))
      return(0);
   for(count = 0; ((strs != NULL)&&(strs[count] != NULL)); count++);
   return(count);
}


inline char *
tinyrad_strings_dequeue(
         char **                       strs,
         int                           freeit )
{
   char *      str;
   size_t      pos;
   TinyRadDebugTrace();
   assert(strs != NULL);
   str = strs[0];
   for(pos = 0; ((strs[pos])); pos++)
      strs[pos] = strs[pos+1];
   strs[pos] = NULL;
   if ((freeit))
   {
      free(str);
      return(NULL);
   };
   return(str);
}


inline intmax_t
tinyrad_strings_dequeue_int(
         char **                       strs )
{
   char *      str;
   intmax_t    i;
   TinyRadDebugTrace();
   assert(strs != NULL);
   str = tinyrad_strings_dequeue(strs, 0);
   i = (intmax_t)strtoll(str, NULL, 0);
   free(str);
   return(i);
}


inline uintmax_t
tinyrad_strings_dequeue_uint(
         char **                       strs )
{
   char *      str;
   uintmax_t   uint;
   TinyRadDebugTrace();
   assert(strs != NULL);
   str = tinyrad_strings_dequeue(strs, 0);
   uint = (uintmax_t)strtoull(str, NULL, 0);
   free(str);
   return(uint);
}


/// Duplicate array of strings
///
/// @param[in]  src           existing array of strings
/// @param[out] destp         pointer to string new array
/// @return returns error code
inline int
tinyrad_strings_dup(
         char **                       src,
         char ***                      destp )
{
   char **     ptr;
   size_t      count;
   size_t      pos;

   TinyRadDebugTrace();

   count  = tinyrad_strings_count( src );

   if ((ptr = malloc((count+1) * sizeof(char*))) == NULL)
      return(TRAD_ENOMEM);

   for(pos = 0; (pos < count); pos++)
   {
      if ((ptr[pos] = strdup(src[pos])) == NULL)
      {
         tinyrad_strings_free(ptr);
         return(TRAD_ENOMEM);
      };
   };
   ptr[pos] = NULL;

   *destp = ptr;

   return(TRAD_SUCCESS);
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  str           string to append to array
/// @return returns error code
inline int
tinyrad_strings_enqueue(
         char ***                      strsp,
         const char *                  str )
{
   TinyRadDebugTrace();
   return(tinyrad_strings_push(strsp, str));
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  i             string to append to array
/// @return returns error code
inline int
tinyrad_strings_enqueue_int(
         char ***                      strsp,
         intmax_t                      i )
{
   TinyRadDebugTrace();
   return(tinyrad_strings_push_int(strsp, i));
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  uint          string to append to array
/// @return returns error code
inline int
tinyrad_strings_enqueue_uint(
         char ***                      strsp,
         uintmax_t                     uint )
{
   TinyRadDebugTrace();
   return(tinyrad_strings_push_uint(strsp, uint));
}


/// Appends string to NULL terminated array of strings
///
/// @param[in]  strs          string array
/// @param[in]  freeit        free string instead of returning it
/// @return returns error code
inline char *
tinyrad_strings_pop(
         char **                       strs,
         int                           freeit )
{
   char *      str;
   size_t      pos;
   TinyRadDebugTrace();
   assert(strs != NULL);
   for(pos = 0; ((strs[pos])); pos++);
   if (!(pos))
      return(NULL);
   str         = strs[pos-1];
   strs[pos-1] = NULL;
   if ((freeit))
   {
      free(str);
      return(NULL);
   };
   return(str);
}


inline intmax_t
tinyrad_strings_pop_int(
         char **                       strs )
{
   intmax_t    i;
   char *      str;
   TinyRadDebugTrace();
   assert(strs != NULL);
   if ((str = tinyrad_strings_pop(strs, 0)) == NULL)
      return(0);
   i = (intmax_t)strtoll(str, NULL, 0);
   free(str);
   return(i);
}


inline uintmax_t
tinyrad_strings_pop_uint(
         char **                       strs )
{
   uintmax_t   uint;
   char *      str;
   TinyRadDebugTrace();
   assert(strs != NULL);
   if ((str = tinyrad_strings_pop(strs, 0)) == NULL)
      return(0);
   uint = (uintmax_t)strtoull(str, NULL, 0);
   free(str);
   return(uint);
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  str           string to append to array
/// @return returns error code
inline int
tinyrad_strings_push(
         char ***                      strsp,
         const char *                  str )
{
   size_t     count;
   char **    strs;

   TinyRadDebugTrace();

   assert(strsp != NULL);
   assert(str   != NULL);

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


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  i             string to append to array
/// @return returns error code
inline int
tinyrad_strings_push_int(
         char ***                      strsp,
         intmax_t                      i )
{
   char str[128];
   TinyRadDebugTrace();
   assert(strsp != NULL);
   snprintf(str, sizeof(str), "%ji", i);
   return(tinyrad_strings_push(strsp, str));
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  uint          string to append to array
/// @return returns error code
inline int
tinyrad_strings_push_uint(
         char ***                      strsp,
         uintmax_t                     uint )
{
   char str[128];
   TinyRadDebugTrace();
   assert(strsp != NULL);
   snprintf(str, sizeof(str), "%ju", uint);
   return(tinyrad_strings_push(strsp, str));
}

#endif /* end of header */
