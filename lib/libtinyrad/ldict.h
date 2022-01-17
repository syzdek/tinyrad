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
#ifndef _LIB_LIBTINYRAD_LDICT_H
#define _LIB_LIBTINYRAD_LDICT_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "libtinyrad.h"

#include <stdatomic.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

struct _tinyrad_dict
{
   atomic_intmax_t       ref_count;
   uint32_t              opts;
   uint32_t              pad32;
   size_t                vendors_len;
   size_t                attrs_len;
   size_t                attrs_type_len;
   size_t                paths_len;
   TinyRadDictVendor **  vendors_name;
   TinyRadDictVendor **  vendors_id;
   TinyRadDictAttr **    attrs_name;
   TinyRadDictAttr **    attrs_type;
   char **               paths;
};


struct _tinyrad_dict_vendor
{
   atomic_intmax_t       ref_count;
   char *                name;
   TinyRadDictAttr **    attrs_name;
   TinyRadDictAttr **    attrs_type;
   size_t                attrs_len;
   uint32_t              id;
   uint8_t               type_octs;
   uint8_t               len_octs;
   uint16_t              pad16;
};


struct _tinyrad_dict_attr
{
   atomic_intmax_t       ref_count;
   char *                name;
   uint32_t              vendor_id;
   uint32_t              type;
   uint8_t               data_type;
   uint8_t               type_octs;
   uint8_t               len_octs;
   uint8_t               pad8;
   uint32_t              flags;
   size_t                values_name_len;
   size_t                values_numeric_len;
   TinyRadDictValue **   values_name;
   TinyRadDictValue **   values_numeric;
};


struct _tinyrad_dict_value
{
   char *                name;
   uint64_t              value;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

TinyRadDictAttr *
tinyrad_dict_attr_lookup(
         TinyRadDict *                dict,
         uint32_t                     vendor_id,
         const char *                 name,
         uint32_t                     id );


TinyRadDictValue *
tinyrad_dict_value_lookup(
         TinyRadDictAttr *            attr,
         const char *                 name,
         uint64_t                     num );


TinyRadDictVendor *
tinyrad_dict_vendor_lookup(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     id );


#endif /* end of header */
