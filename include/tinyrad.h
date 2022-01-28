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
#include <sys/types.h>


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
#define TRAD_ENOBUFS                0x0006 ///< no buffer space available
#define TRAD_EEXISTS                0x0007 ///< dictionary object exists
#define TRAD_EURL                   0x0008 ///< invalid RADIUS URL
#define TRAD_ERESOLVE               0x0009 ///< unable to resolve host
#define TRAD_EOPTERR                0x000a ///< option error
#define TRAD_ECONNECT               0x000b ///< connection error
#define TRAD_ESCHEME                0x000C ///< invalid or mismatched URL scheme
#define TRAD_EINVAL                 0x000D ///< invalid argument

// library user options
#define TRAD_OPTS_USER              0x000FFFFFU
#define TRAD_IPV4                   0x00000004U
#define TRAD_IPV6                   0x00000008U
#define TRAD_IP_UNSPEC              (TRAD_IPV4 | TRAD_IPV6)
#define TRAD_CATHOLIC               0x00000010U
#define TRAD_SERVER                 0x00000020U

// library internal options
#define TRAD_OPTS_INTERNAL          (~TRAD_OPTS_USER)
#define TRAD_RADIUS                 0x00100000U
#define TRAD_RADIUS_ACCT            0x00200000U
#define TRAD_RADIUS_DYNAUTH         0x00400000U
#define TRAD_TLS                    0x00800000U
#define TRAD_SCHEME                 0x00F00000U
#define TRAD_RADSEC                 (TRAD_RADIUS | TRAD_RADIUS_ACCT | TRAD_RADIUS_DYNAUTH | TRAD_TLS)
#define TRAD_TCP                    0x01000000U

#define TRAD_OFF                    0
#define TRAD_ON                     1

// library options
#define TRAD_OPT_IPV4                   1
#define TRAD_OPT_DESC                   2
#define TRAD_OPT_URI                    3
#define TRAD_OPT_DEBUG_LEVEL            4
#define TRAD_OPT_IPV6                   5
#define TRAD_OPT_DIAGNOSTIC_MESSAGE     6
#define TRAD_OPT_NETWORK_TIMEOUT        7
#define TRAD_OPT_SOCKET_BIND_ADDRESSES  8
#define TRAD_OPT_TIMEOUT                9
#define TRAD_OPT_DEBUG_IDENT           10
#define TRAD_OPT_DEBUG_SYSLOG          11
#define TRAD_OPT_SCHEME                12

// dictionary options
#define TRAD_DICT_OPT_REF_COUNT           1  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_NAME                2  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_TYPE                3  // used by TinyRadDictAttr and TinyRadDictValue
#define TRAD_DICT_OPT_VEND_ID             4  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_VEND_TYPE           5  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_TYPE_OCTS           6  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_LEN_OCTS            7  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_DATA_TYPE           8  // used by TinyRadDictAttr
#define TRAD_DICT_OPT_FLAGS               9  // used by TinyRadDictAttr
#define TRAD_DICT_OPT_VALUE              10  // used by TinyRadDictValue

// library debug levels
#define TRAD_DEBUG_NONE             0
#define TRAD_DEBUG_TRACE            0x0000001
#define TRAD_DEBUG_ARGS             0x0000002
#define TRAD_DEBUG_CONNS            0x0000004
#define TRAD_DEBUG_PACKETS          0x0000008
#define TRAD_DEBUG_PARSE            0x0000010
#define TRAD_DEBUG_ANY              (~0x00)

// library defaults
#define TRAD_DFLT_DEBUG_IDENT             "libtinyrad"
#define TRAD_DFLT_DEBUG_LEVEL             TRAD_DEBUG_NONE
#define TRAD_DFLT_DEBUG_SYSLOG            TRAD_OFF
#define TRAD_DFLT_TIMEOUT                 10
#define TRAD_DFLT_NET_TIMEOUT_SEC         10
#define TRAD_DFLT_NET_TIMEOUT_USEC        0
#define TRAD_DFLT_SOCKET_BIND_ADDRESSES   "0.0.0.0 ::"

#define TRAD_RADSEC_SECRET_TCP      "radsec"       // RFC 6614 Section 2.3: Connection Setup
#define TRAD_RADSEC_SECRET_UDP      "radius/dtls"  // RFC 7360 Section 2.1: Changes to RADIUS

// RADIUS Attribute Types
#define TRAD_STRING                 1 ///< UTF-8 printable text (the RFCs call this "text")
#define TRAD_OCTETS                 2 ///< opaque binary data (the RFCs call this "string")
#define TRAD_IPADDR                 3 ///< IPv4 address
#define TRAD_DATE                   4 ///< Seconds since January 1, 1970 (32-bits)
#define TRAD_INTEGER                5 ///< 32-bit unsigned integer
#define TRAD_IPV6ADDR               6 ///< IPv6 Address
#define TRAD_IPV6PREFIX             7 ///< IPV6 prefix, with mask
#define TRAD_IFID                   8 ///< Interface Id (hex:hex:hex:hex)
#define TRAD_INTEGER64              9 ///< 64-bit unsigned integer
#define TRAD_ETHER                 10 ///< Ethernet MAC address
#define TRAD_ABINARY               11 ///< Ascend binary filter format
#define TRAD_BYTE                  12 ///< 8-bit unsigned integer
#define TRAD_SHORT                 13 ///< 16-bit unsigned integer
#define TRAD_SIGNED                14 ///< 31-bit signed integer (packed into 32-bit field)
#define TRAD_TLV                   15 ///< Type-Length-Value, allows nested attributes (RFC 6929: Section 2.3 TLV Data Type)
#define TRAD_IPV4PREFIX            16 ///< IPv4 Prefix as given in RFC 6572.
#define TRAD_VSA                   17 ///< Vendor specific attribute
#define TRAD_EXTENDED              18 ///< Extended Type (RFC 6929: Section 2.1 Extended Type)
#define TRAD_LONGEXTENDED          19 ///< Long Extended Type (RFC 6929: Section 2.2 Long Extended Type)


// RADIUS Attribute flags
#define TRAD_ENCRYPT1               0x0001 ///< encrypted with the method as defined in RFC2865 for the User-Password attribute
#define TRAD_ENCRYPT2               0x0002 ///< encrypted with the method as defined in RFC2868 for the Tunnel-Password attribute
#define TRAD_ENCRYPT3               0x0004 ///< encrypted as per Ascend's definitions for the Ascend-Send-Secret attribute
#define TRAD_ENCRYPT_MASK           0x00FF ///< encrypt method mask
#define TRAD_HAS_TAG                0x0100 ///< attribute may have tag as defined in RFC 2868
#define TRAD_CONCAT                 0x0200 ///< concatenate values
#define TRAD_DFLT_ATTR              0x1000 ///< default/built-in attribute


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
typedef struct _tinyrad_dict           TinyRadDict;
typedef struct _tinyrad_dict_vendor    TinyRadDictVendor;
typedef struct _tinyrad_dict_attr      TinyRadDictAttr;
typedef struct _tinyrad_dict_value     TinyRadDictValue;
typedef struct _tinyrad_map            TinyRadMap;
typedef struct tinyrad_url_desc        TinyRadURLDesc;
typedef struct sockaddr_storage        TinyRadSockAddr;


// Support RADIUS URLs
//    radius://hostport/secret[?proto]          (default proto: udp, port: 1812) [RFC2865]
//    radius-acct://hostport/secret[?proto]     (default proto: udp, port: 1813) [RFC2866]
//    radsec://hostport/[?proto]                (default proto: tcp, port: 2083) [RFC6614/RFC7360]
//    radius-dynauth://hostport/secret[?proto]  (default proto: udp, port: 3799) [RFC5176]
struct tinyrad_url_desc
{
   char *               trud_host;
   char *               trud_secret;
   int                  trud_port;
   unsigned             trud_opts;
   /* may contain additional fields for internal use */
   TinyRadURLDesc *     trud_next;
   TinyRadSockAddr **   sockaddrs;
   size_t               sockaddrs_len;
};


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
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr );


_TINYRAD_F TinyRadDictAttr *
tinyrad_dict_attr_get(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type );


_TINYRAD_F int
tinyrad_dict_attr_info(
         TinyRadDictAttr *             attr,
         int                           param,
         void *                        outvalue );


_TINYRAD_F int
tinyrad_dict_defaults(
         TinyRadDict *                 dict,
         char ***                      msgsp,
         uint32_t                      opts );


_TINYRAD_F void
tinyrad_dict_destroy(
         TinyRadDict *                 dict );


_TINYRAD_F int
tinyrad_dict_initialize(
         TinyRadDict **                dictp,
         uint32_t                      opts );


_TINYRAD_F int
tinyrad_dict_parse(
         TinyRadDict *                 dict,
         const char *                  file,
         char ***                      msgsp,
         uint32_t                      opts );


_TINYRAD_F void
tinyrad_dict_print(
         TinyRadDict *                 dict,
         uint32_t                      opts );


_TINYRAD_F int
tinyrad_dict_value_info(
         TinyRadDictValue *            value,
         int                           param,
         void *                        outvalue );


_TINYRAD_F TinyRadDictValue *
tinyrad_dict_value_lookup(
         TinyRadDictAttr *            attr,
         const char *                 name,
         uint64_t                     num );


_TINYRAD_F void
tinyrad_dict_vendor_destroy(
         TinyRadDictVendor *           vendor );


_TINYRAD_F TinyRadDictVendor *
tinyrad_dict_vendor_get(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     vendor_id );


_TINYRAD_F int
tinyrad_dict_vendor_info(
         TinyRadDictVendor *           vendor,
         int                           option,
         void *                        outvalue );


//------------------//
// error functions //
//------------------//
#pragma mark error functions

_TINYRAD_F const char *
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


_TINYRAD_F void
tinyrad_free(
         void *                        ptr );


_TINYRAD_F int
tinyrad_get_option(
         TinyRad *                     tr,
         int                           option,
         void *                        outvalue );


_TINYRAD_F int
tinyrad_initialize(
         TinyRad **                    trp,
         const char *                  url,
         uint64_t                      opts );


_TINYRAD_F int
tinyrad_set_option(
         TinyRad *                     tr,
         int                           option,
         const void *                  invalue );


_TINYRAD_F void
tinyrad_strings_free(
         char **                       strs );


//---------------//
// URL functions //
//---------------//
#pragma mark URL functions

_TINYRAD_F int
tinyrad_is_radius_url(
         const char *                  url );


_TINYRAD_F char *
tinyrad_urldesc2str(
         TinyRadURLDesc *              trudp );


_TINYRAD_F int
tinyrad_urldesc_alloc(
         TinyRadURLDesc **             trudpp );


_TINYRAD_F void
tinyrad_urldesc_free(
         TinyRadURLDesc *              trudp );


_TINYRAD_F int
tinyrad_urldesc_parse(
         const char *                  url,
         TinyRadURLDesc **             trudpp );


_TINYRAD_F int
tinyrad_urldesc_resolve(
         TinyRadURLDesc *              trudp,
         uint32_t                      opts );


_TINYRAD_F unsigned
tinyrad_urldesc_scheme(
         TinyRadURLDesc *             trud );

#endif /* end of header */
