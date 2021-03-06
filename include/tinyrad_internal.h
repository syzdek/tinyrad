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
#ifndef __TINYRAD_INTERNAL_H
#define __TINYRAD_INTERNAL_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stddef.h>
#include <inttypes.h>
#include <sys/types.h>

#include <tinyrad.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#pragma mark - Macros


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#define TRAD_LINE_MAX_LEN           256

// array function options
#define TINYRAD_ARRAY_INSERT        0x0001      ///< add type: insert unique object to sorted array
#define TINYRAD_ARRAY_REPLACE       0x0002      ///< add type: replace deplucate object in sorted array, or insert if unique
#define TINYRAD_ARRAY_MERGE         0x0004      ///< add type: add object to sorted array regardless if unique or duplicate
#define TINYRAD_ARRAY_LASTDUP       0x0010      ///< dup handling: add/return/remove last duplicate object in series of matching objects
#define TINYRAD_ARRAY_FIRSTDUP      0x0020      ///< dup handling: add/return/remove first duplicate object in series of matching objects
#define TINYRAD_ARRAY_ANYDUP        0x0040      ///< dup handling: add/return/remove any one duplicate object in series of matching objects
#define TINYRAD_ARRAY_DEFAULT       ( TINYRAD_ARRAY_INSERT | TINYRAD_ARRAY_ANYDUP )                            ///< default sorted array options
#define TINYRAD_ARRAY_MASK_ADD      ( TINYRAD_ARRAY_INSERT | TINYRAD_ARRAY_MERGE   | TINYRAD_ARRAY_REPLACE )   ///< mask for add type
#define TINYRAD_ARRAY_MASK_DUPS     ( TINYRAD_ARRAY_ANYDUP | TINYRAD_ARRAY_LASTDUP | TINYRAD_ARRAY_FIRSTDUP )  ///< mask for duplicate handling in sorted array
#define TINYRAD_ARRAY_MASK          ( TINYRAD_ARRAY_MASK_ACTION | TINYRAD_ARRAY_MASK_DUPS )                    ///< mask of all sorted array options


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

extern const TinyRadDictVendorDef      tinyrad_dict_default_vendors[];
extern const TinyRadDictAttrDef        tinyrad_dict_default_attrs[];
extern const TinyRadDictValueDef       tinyrad_dict_default_values[];


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//------------------//
// array prototypes //
//------------------//
#pragma mark array prototypes

_TINYRAD_F ssize_t
tinyrad_array_add(
         void **                       basep,
         size_t *                      nelp,
         size_t                        width,
         void *                        obj,
         unsigned                      opts,
         int (*compar)(const void *, const void *),
         void (*freeobj)(void *),
         void * (*reallocbase)(void *, size_t) );


_TINYRAD_F void *
tinyrad_array_dequeue(
         void *                        base,
         size_t *                      nelp,
         size_t                        width );


_TINYRAD_F ssize_t
tinyrad_array_enqueue(
         void **                       basep,
         size_t *                      nelp,
         size_t                        width,
         void *                        obj,
         void * (*reallocbase)(void *, size_t) );


_TINYRAD_F void *
tinyrad_array_get(
         void *                        base,
         size_t                        nel,
         size_t                        width,
         const void *                  key,
         unsigned                      opts,
         int (*compar)(const void *, const void *) );


_TINYRAD_F void *
tinyrad_array_peek(
         void *                        base,
         size_t                        nel,
         size_t                        width );


_TINYRAD_F void *
tinyrad_array_pop(
         void *                        base,
         size_t *                      nelp,
         size_t                        width );


_TINYRAD_F ssize_t
tinyrad_array_push(
         void **                       basep,
         size_t *                      nelp,
         size_t                        width,
         void *                        obj,
         void * (*reallocbase)(void *, size_t) );


_TINYRAD_F ssize_t
tinyrad_array_remove(
         void *                        base,
         size_t *                      nelp,
         size_t                        width,
         const void *                  key,
         unsigned                      opts,
         int (*compar)(const void *, const void *),
         void (*freeobj)(void *) );


_TINYRAD_F ssize_t
tinyrad_array_search(
         const void *                  base,
         size_t                        nel,
         size_t                        width,
         const void *                  key,
         unsigned                      opts,
         size_t *                      wouldbep,
         int (*compar)(const void *, const void *) );


//------------------//
// debug prototypes //
//------------------//
#pragma mark debug prototypes

_TINYRAD_F void *
tinyrad_assertions( void );


//-----------------//
// file prototypes //
//-----------------//
#pragma mark file prototypes

_TINYRAD_F int
tinyrad_readline(
         int                           fd,
         char *                        str,
         size_t                        size,
         size_t *                      bytes_read );


//--------------------//
// strings prototypes //
//--------------------//
#pragma mark strings prototypes

_TINYRAD_F char *
tinyrad_strdup(
         const char *                  s1 );


_TINYRAD_F char *
tinyrad_strexpand(
         char *                        dst,
         const char * restrict         src,
         size_t                        len,
         int                           force_expansion );


_TINYRAD_F size_t
tinyrad_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


_TINYRAD_F size_t
tinyrad_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


_TINYRAD_F char *
tinyrad_strndup(
         const char *                  s1,
         size_t                        n );


_TINYRAD_F int
tinyrad_strsadd(
         char ***                      strsp,
         const char *                  str );


_TINYRAD_F size_t
tinyrad_strscount(
         char * const *                strs );


_TINYRAD_F int
tinyrad_strsdup(
         char ***                      dstp,
         char * const *                src );


_TINYRAD_F int
tinyrad_strsplit(
         const char *                  str,
         int                           delim,
         char ***                      argvp,
         int *                         argcp );


_TINYRAD_F int
tinyrad_strtoargs(
         char *                        str,
         char ***                      argvp,
         int *                         argcp );


_TINYRAD_F int
tinyrad_strtobool(
         const char *                  str );


_TINYRAD_F char *
tinyrad_strtrim(
         char *                        str );


#endif /* end of header */
