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
#ifndef __TINYRAD_H
#define __TINYRAD_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stddef.h>
#include <inttypes.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#pragma mark - Macros

// Exports function type
#undef TINYRAD_C_DECLS
#undef TINYRAD_BEGIN_C_DECLS
#undef TINYRAD_END_C_DECLS
#undef _TINYRAD_I
#undef _TINYRAD_F
#undef _TINYRAD_V
#if defined(__cplusplus) || defined(c_plusplus)
#   define TINYRAD_C_DECLS        "C"             ///< exports as C functions
#   define TINYRAD_BEGIN_C_DECLS  extern "C" {    ///< exports as C functions
#   define TINYRAD_END_C_DECLS    }               ///< exports as C functions
#else
#   define TINYRAD_C_DECLS        /* empty */     ///< exports as C functions
#   define TINYRAD_BEGIN_C_DECLS  /* empty */     ///< exports as C functions
#   define TINYRAD_END_C_DECLS    /* empty */     ///< exports as C functions
#endif
#ifdef WIN32
#   ifdef _LIB_LIBTINYRAD_H
#      define _TINYRAD_I   inline
#      define _TINYRAD_F   extern TINYRAD_C_DECLS __declspec(dllexport)   ///< used for library calls
#      define _TINYRAD_V   extern TINYRAD_C_DECLS __declspec(dllexport)   ///< used for library calls
#   else
#      define _TINYRAD_I   extern TINYRAD_C_DECLS __declspec(dllimport)   ///< used for library calls
#      define _TINYRAD_F   extern TINYRAD_C_DECLS __declspec(dllimport)   ///< used for library calls
#      define _TINYRAD_V   extern TINYRAD_C_DECLS __declspec(dllimport)   ///< used for library calls
#   endif
#else
#   ifdef _LIB_LIBTINYRAD_H
#      define _TINYRAD_I   inline
#      define _TINYRAD_F   /* empty */                                    ///< used for library calls
#      define _TINYRAD_V   extern TINYRAD_C_DECLS                         ///< used for library calls
#   else
#      define _TINYRAD_I   extern TINYRAD_C_DECLS                         ///< used for library calls
#      define _TINYRAD_F   extern TINYRAD_C_DECLS                         ///< used for library calls
#      define _TINYRAD_V   extern TINYRAD_C_DECLS                         ///< used for library calls
#   endif
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#define TRAD_SUCCESS                0x0000
#define TRAD_EUNKNOWN               0x0001 ///< unknown error
#define TRAD_ENOMEM                 0x0002 ///< out of virtual memory
#define TRAD_EACCES                 0x0003 ///< permission denied
#define TRAD_ENOENT                 0x0004 ///< no such file or directory
#define TRAD_ESYNTAX                0x0005 ///< dictionary syntax error


// library options
#define TRAD_TCP                    0x0001
#define TRAD_BUILTIN                0x0002
#define TRAD_CATHOLIC               0x0004


// RADIUS Attribute Types
#define TRAD_STRING                 1      ///< UTF-8 printable text (the RFCs call this "text")
#define TRAD_OCTETS                 2      ///< opaque binary data (the RFCs call this "string")
#define TRAD_IPADDR                 3      ///< IPv4 address
#define TRAD_DATE                   4      ///< Seconds since January 1, 1970 (32-bits)
#define TRAD_INTEGER                5      ///< 32-bit unsigned integer
#define TRAD_IPV6ADDR               6      ///< IPv6 Address
#define TRAD_IPV6PREFIX             7      ///< IPV6 prefix, with mask
#define TRAD_IFID                   8      ///< Interface Id (hex:hex:hex:hex)
#define TRAD_INTEGER64              9      ///< 64-bit unsigned integer
#define TRAD_ETHER                 10      ///< Ethernet MAC address
#define TRAD_ABINARY               11      ////< Ascend binary filter format
#define TRAD_BYTE                  12      ////< 8-bit unsigned integer
#define TRAD_SHORT                 13      ////< 16-bit unsigned integer
#define TRAD_SIGNED                14      ////< 31-bit signed integer (packed into 32-bit field)
#define TRAD_TLV                   15      ////< Type-Length-Value (allows nested attributes)
#define TRAD_IPV4PREFIX            16      ////< IPv4 Prefix as given in RFC 6572.


// RADIUS codes
#define TRAD_ACCESS_REQ             1
#define TRAD_ACCESS_ACCEPT          2
#define TRAD_ACCESS_REJECT          3
#define TRAD_ACCOUNT_REQ            4
#define TRAD_ACCOUNT_RES            5
#define TRAD_ACCESS_CHALLENGE       11
#define TRAD_STATUS_SERVER          12
#define TRAD_STATUS_CLIENT          13
#define TRAD_DISCONNECT_REQ         40
#define TRAD_DISCONNECT_ACK         41
#define TRAD_DISCONNECT_NAK         42
#define TRAD_COA_REQ                43
#define TRAD_COA_ACK                44
#define TRAD_COA_NAK                45


// Tiny RADIUS Crypto Methods
#define TRAD_MD5                    1
#define TRAD_SHA1                   2
#define TRAD_SHA512                 3


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _tinyrad                TinyRad;
typedef struct _tinyrad_avp            TinyRadAVP;
typedef struct _tinyrad_dict           TinyRadDict;
typedef struct _tinyrad_dict_vsa       TinyRadDictVSA;
typedef struct _tinyrad_dict_vendor    TinyRadDictVendor;
typedef struct _tinyrad_dict_attr      TinyRadDictAttr;
typedef struct _tinyrad_dict_val       TinyRadDictVal;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//----------------------//
// dictionary functions //
//----------------------//
#pragma mark dictionary functions

_TINYRAD_F int
tinyrad_dict_add_path(
         TinyRadDict *                 dict,
         const char *                  path );


_TINYRAD_F void
tinyrad_dict_destroy(
         TinyRadDict *                 dict );


_TINYRAD_F int
tinyrad_dict_import(
         TinyRadDict *                 dict,
         const char *                  file,
         char ***                      msgsp,
         uint32_t                      opts );


_TINYRAD_F int
tinyrad_dict_initialize(
         TinyRadDict **                dictp,
         uint32_t                      opts );


//------------------//
// error functions //
//------------------//
#pragma mark error functions

_TINYRAD_F char *
tinyrad_strerror(
         int                           errnum );


_TINYRAD_F int
tinyrad_strerror_r(
         int                           errnum,
         char *                        strerrbuf,
         size_t                        buflen );


//------------------//
// memory functions //
//------------------//
#pragma mark memory functions


_TINYRAD_F void
tinyrad_destroy(
         TinyRad *                     tr );


_TINYRAD_F int
tinyrad_initialize(
         TinyRad **                    trp,
         const char *                  server,
         uint64_t                      opts );


_TINYRAD_I void
tinyrad_strings_free(
         char **                       strs );


#endif /* end of header */
