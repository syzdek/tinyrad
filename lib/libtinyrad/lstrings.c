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
#define _LIB_LIBTINYRAD_LSTRINGS_C 1
#include "lstrings.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>


////////////////////////
//                    //
//  Inline Functions  //
//                    //
////////////////////////
#pragma mark - Inline Functions

extern inline size_t
tinyrad_strings_count(
         char **                       strs );


extern inline int
tinyrad_strings_dup(
         char **                       src,
         char ***                      destp );


extern inline int
tinyrad_strings_enqueue(
         char ***                      strsp,
         const char *                  str );


extern inline int
tinyrad_strings_enqueue_int(
         char ***                      strsp,
         intmax_t                      i );


extern inline int
tinyrad_strings_enqueue_uint(
         char ***                      strsp,
         uintmax_t                     uint );


extern inline void
tinyrad_strings_free(
         char **                       strs );


extern inline char *
tinyrad_strings_pop(
         char **                       strs,
         int                           freeit );


inline intmax_t
tinyrad_strings_pop_int(
         char **                       strs );


inline uintmax_t
tinyrad_strings_pop_uint(
         char **                       strs );


extern inline int
tinyrad_strings_push(
         char ***                      strsp,
         const char *                  str );


extern inline int
tinyrad_strings_push_int(
         char ***                      strsp,
         intmax_t                      i );


extern inline int
tinyrad_strings_push_uint(
         char ***                      strsp,
         uintmax_t                     uint );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

/* end of source */
