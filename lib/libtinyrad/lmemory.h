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

//-------------------//
// object prototypes //
//-------------------//
#pragma mark object prototypes

void *
tinyrad_obj_alloc(
         size_t                        size,
         void (*free_func)(void * ptr) );


void
tinyrad_obj_release(
         TinyRadObj *                  obj );


void *
tinyrad_obj_retain(
         TinyRadObj *                  obj );


intptr_t
tinyrad_obj_retain_count(
         TinyRadObj *                  obj );


//----------------//
// OID prototypes //
//----------------//
#pragma mark OID prototypes

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

char *
tinyrad_strdup(
         const char *                  s1 );


size_t
tinyrad_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


size_t
tinyrad_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


char *
tinyrad_strndup(
         const char *                  s1,
         size_t                        n );


int
tinyrad_strsadd(
         char ***                      strsp,
         const char *                  str );


size_t
tinyrad_strscount(
         char **                       strs );


#endif /* end of header */
