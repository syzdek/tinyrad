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
#ifndef __TINYRAD_COMMON_H
#define __TINYRAD_COMMON_H 1


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


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _tinyrad_dict_attr_def     TinyRadDictAttrDef;
typedef struct _tinyrad_dict_value_def    TinyRadDictValueDef;
typedef struct _tinyrad_dict_vendor_def   TinyRadDictVendorDef;


struct _tinyrad_dict_attr_def
{
   const char *          name;
   uintptr_t             type;
   uint32_t              vendor_id;
   uint32_t              vendor_type;
   uint32_t              data_type;
   uint32_t              flags;
};


struct _tinyrad_dict_value_def
{
   const char *          attr_name;
   const char *          value_name;
   uint64_t              data;
};


struct _tinyrad_dict_vendor_def
{
   const char *          name;
   uintptr_t             vendor_id;
   uint64_t              vendor_type_octs;
   uint64_t              vendor_len_octs;
};


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

//-----------------//
// array functions //
//-----------------//
#pragma mark array functions

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


//----------------------//
// dictionary functions //
//----------------------//
#pragma mark dictionary functions

_TINYRAD_F int
tinyrad_dict_import(
         TinyRadDict *                 dict,
         const TinyRadDictVendorDef *  vendor_defs,
         const TinyRadDictAttrDef *    attr_defs,
         const TinyRadDictValueDef *   value_defs,
         char ***                      msgsp,
         uint32_t                      opts );


#endif /* end of header */
