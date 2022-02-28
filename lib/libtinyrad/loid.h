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
#ifndef _LIB_LIBTINYRAD_LOID_H
#define _LIB_LIBTINYRAD_LOID_H 1


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
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

// According to RFC 6929 Section 2.3.1, "nesting [TLV] depths of more than 4
// are NOT RECOMMENDED."  Using the RFC recommendation, an OID for a TLV value
// within a Vendor-Specific attribute would have at most 6 values (Attribute
// Type, Vendor-ID, and up to 4 TLV-Types). Attributes of the Extended-Type
// and Long-Extended-Type have an additional value for "Extended-Type".
// TinyRad uses a max OID length of 7 to allow a depth of 4 TLV values in
// Extended-Type and Long-Extended-Type attributes and to prevent struct
// padding by having an odd number of uint32_t values.
#define TRAD_OID_MAX_LEN   7 // use odd value to avoid struct padding
struct _tinyrad_oid
{
    uint32_t            oid_len;
    uint32_t            oid_val[TRAD_OID_MAX_LEN];
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//-------------------//
// memory prototypes //
//-------------------//
#pragma mark memory prototypes

TinyRadOID *
tinyrad_oid_alloc(
         void );


TinyRadOID *
tinyrad_oid_dup(
         const TinyRadOID *            ptr );


uint32_t
tinyrad_oid_pop(
         TinyRadOID *                  oid );


int
tinyrad_oid_push(
         TinyRadOID *                  oid,
         uint32_t                      val );


//------------------//
// query prototypes //
//------------------//
#pragma mark query prototypes

uint32_t
tinyrad_oid_type(
         const TinyRadOID *            oid );


uint32_t
tinyrad_oid_vendor_id(
         const TinyRadOID *            oid );


uint32_t
tinyrad_oid_vendor_type(
         const TinyRadOID *            oid );


//-------------------//
// string prototypes //
//-------------------//
#pragma mark string prototypes

TinyRadOID *
tinyrad_str2oid(
         const char *                  str );


#endif /* end of header */
