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
#define _LIB_LIBTINYRAD_LDICT_C 1
#include "ldict.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "lerror.h"
#include "lfile.h"
#include "lmemory.h"
#include "lmap.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#define TRAD_DICT_INCLUDE           1
#define TRAD_DICT_ATTRIBUTE         2
#define TRAD_DICT_VALUE             3
#define TRAD_DICT_VENDOR            4
#define TRAD_DICT_BEGIN_VENDOR      5
#define TRAD_DICT_END_VENDOR        6


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tinyrad_dict_attr_add(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor,
         TinyRadDictAttr **            attrp,
         const char *                  name,
         uint32_t                      type,
         uint8_t                       datatype,
         uint32_t                      flags );


int
tinyrad_dict_attr_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 );


int
tinyrad_dict_attr_cmp_type(
         const void *                 ptr1,
         const void *                 ptr2 );


void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr );


int
tinyrad_dict_attr_lookup_name(
         const void *                 data,
         const void *                 idx );


int
tinyrad_dict_attr_lookup_type(
         const void *                 data,
         const void *                 idx );


int
tinyrad_dict_import_attribute(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor,
         TinyRadFile *                file,
         uint32_t                     opts );


int
tinyrad_dict_import_begin_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file );


int
tinyrad_dict_import_end_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file );


int
tinyrad_dict_import_include(
         TinyRadDict *                dict,
         TinyRadFile **               filep );


int
tinyrad_dict_import_value(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts );


int
tinyrad_dict_import_vendor(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts );


void *
tinyrad_dict_lookup(
         void **                      list,
         size_t                       len,
         const void *                 idx,
         int (*compar)(const void *, const void *) );


void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr );


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor );


int
tinyrad_dict_value_add(
         TinyRadDictAttr *             attr,
         TinyRadDictValue **           valuep,
         const char *                  name,
         uint64_t                      numeral );


int
tinyrad_dict_value_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 );


int
tinyrad_dict_value_cmp_numeric(
         const void *                 ptr1,
         const void *                 ptr2 );


void
tinyrad_dict_value_destroy(
         TinyRadDictValue *           value );


int
tinyrad_dict_value_lookup_name(
         const void *                 data,
         const void *                 idx );


int
tinyrad_dict_value_lookup_numeric(
         const void *                 data,
         const void *                 idx );


int
tinyrad_dict_vendor_add(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         const char *                 name,
         uint32_t                     id,
         uint8_t                      type_octs,
         uint8_t                      len_octs );


int
tinyrad_dict_vendor_cmp_id(
         const void *                 ptr1,
         const void *                 ptr2 );


int
tinyrad_dict_vendor_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 );


void
tinyrad_dict_vendor_destroy(
         TinyRadDictVendor *           vendor );


int
tinyrad_dict_vendor_lookup_id(
         const void *                 data,
         const void *                 idx );


int
tinyrad_dict_vendor_lookup_name(
         const void *                 data,
         const void *                 idx );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const TinyRadMap tinyrad_dict_data_type[] =
{
   { "ABINARY",      TRAD_ABINARY},
   { "BYTE",         TRAD_BYTE},
   { "DATE",         TRAD_DATE},
   { "ETHER",        TRAD_ETHER},
   { "IFID",         TRAD_IFID},
   { "INTEGER",      TRAD_INTEGER},
   { "INTEGER64",    TRAD_INTEGER64},
   { "IPADDR",       TRAD_IPADDR},
   { "IPV4PREFIX",   TRAD_IPV4PREFIX},
   { "IPV6ADDR ",    TRAD_IPV6ADDR},
   { "IPV6PREFIX ",  TRAD_IPV6PREFIX},
   { "OCTETS",       TRAD_OCTETS},
   { "RAD_TLV",      TRAD_TLV},
   { "SHORT",        TRAD_SHORT},
   { "SIGNED",       TRAD_SIGNED},
   { "STRING",       TRAD_STRING },
   { "VSA",          TRAD_VSA },
   { NULL, 0 }
};


static const TinyRadMap tinyrad_dict_attr_flags[] =
{
   { "encrypt=1",    TRAD_ENCRYPT1},
   { "encrypt=2",    TRAD_ENCRYPT2},
   { "encrypt=3",    TRAD_ENCRYPT3},
   { "has_tag",      TRAD_HAS_TAG},
   { NULL, 0 }
};


static const TinyRadMap tinyrad_dict_options[] =
{
   { "$INCLUDE",        TRAD_DICT_INCLUDE},
   { "ATTRIBUTE",       TRAD_DICT_ATTRIBUTE},
   { "BEGIN-VENDOR",    TRAD_DICT_BEGIN_VENDOR},
   { "END-VENDOR",      TRAD_DICT_END_VENDOR},
   { "VALUE",           TRAD_DICT_VALUE},
   { "VENDOR",          TRAD_DICT_VENDOR},
   { NULL, 0 }
};


static const struct
{
   const char *          name;
   uint64_t              type;
   uint64_t              data_type;
   uint64_t              flags;
} tinyrad_dict_default_attrs[] =
{
   { "User-Name",                  1,   TRAD_STRING,   0 },              // RFC2865 Section 5.1  User-Name
   { "User-Password",              2,   TRAD_STRING,   TRAD_ENCRYPT1 },  // RFC2865 Section 5.2  User-Password
   { "CHAP-Password",              3,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.3   CHAP-Password
   { "NAS-IP-Address",             4,   TRAD_IPADDR,   0 },              // RFC2865 Section 5.4   NAS-IP-Address
   { "NAS-Port",                   5,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.5   NAS-Port
   { "Service-Type",               6,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.6   Service-Type
   { "Framed-Protocol",            7,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.7   Framed-Protocol
   { "Framed-IP-Address",          8,   TRAD_IPADDR,   0 },              // RFC2865 Section 5.8   Framed-IP-Address
   { "Framed-IP-Netmask",          9,   TRAD_IPADDR,   0 },              // RFC2865 Section 5.9   Framed-IP-Netmask
   { "Framed-Routing",            10,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.10  Framed-Routing
   { "Filter-Id",                 11,   TRAD_STRING,   0 },              // RFC2865 Section 5.11  Filter-Id
   { "Framed-MTU",                12,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.12  Framed-MTU
   { "Framed-Compression",        13,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.13  Framed-Compression
   { "Login-IP-Host",             14,   TRAD_IPADDR,   0 },              // RFC2865 Section 5.14  Login-IP-Host
   { "Login-Service",             15,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.15  Login-Service
   { "Login-TCP-Port",            16,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.16  Login-TCP-Port
   { "Reply-Message",             18,   TRAD_STRING,   0 },              // RFC2865 Section 5.18  Reply-Message
   { "Callback-Number",           19,   TRAD_STRING,   0 },              // RFC2865 Section 5.19  Callback-Number
   { "Callback-Id",               20,   TRAD_STRING,   0 },              // RFC2865 Section 5.20  Callback-Id
   { "Framed-Route",              22,   TRAD_STRING,   0 },              // RFC2865 Section 5.22  Framed-Route
   { "Framed-IPX-Network",        23,   TRAD_IPADDR,   0 },              // RFC2865 Section 5.23  Framed-IPX-Network
   { "State",                     24,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.24  State
   { "Class",                     25,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.25  Class
   { "Vendor-Specific",           26,   TRAD_VSA,      0 },              // RFC2865 Section 5.26  Vendor-Specific
   { "Session-Timeout",           27,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.27  Session-Timeout
   { "Idle-Timeout",              28,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.28  Idle-Timeout
   { "Termination-Action",        29,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.29  Termination-Action
   { "Called-Station-Id",         30,   TRAD_STRING,   0 },              // RFC2865 Section 5.30  Called-Station-Id
   { "Calling-Station-Id",        31,   TRAD_STRING,   0 },              // RFC2865 Section 5.31  Calling-Station-Id
   { "NAS-Identifier",            32,   TRAD_STRING,   0 },              // RFC2865 Section 5.32  NAS-Identifier
   { "Proxy-State",               33,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.33  Proxy-State
   { "Login-LAT-Service",         34,   TRAD_STRING,   0 },              // RFC2865 Section 5.34  Login-LAT-Service
   { "Login-LAT-Node",            35,   TRAD_STRING,   0 },              // RFC2865 Section 5.35  Login-LAT-Node
   { "Login-LAT-Group",           36,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.36  Login-LAT-Group
   { "Framed-AppleTalk-Link",     37,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.37  Framed-AppleTalk-Link
   { "Framed-AppleTalk-Network",  38,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.38  Framed-AppleTalk-Network
   { "Framed-AppleTalk-Zone",     39,   TRAD_STRING,   0 },              // RFC2865 Section 5.39  Framed-AppleTalk-Zone
   { "Acct-Status-Type",          40,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.1   Acct-Status-Type
   { "Acct-Delay-Time",           41,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.2   Acct-Delay-Time
   { "Acct-Input-Octets",         42,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.3   Acct-Input-Octets
   { "Acct-Output-Octets",        43,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.4   Acct-Output-Octets
   { "Acct-Session-Id",           44,   TRAD_STRING,   0 },              // RFC2866 Section 5.5   Acct-Session-Id
   { "Acct-Authentic",            45,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.6   Acct-Authentic
   { "Acct-Session-Time",         46,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.7   Acct-Session-Time
   { "Acct-Input-Packets",        47,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.8   Acct-Input-Packets
   { "Acct-Output-Packets",       48,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.9   Acct-Output-Packets
   { "Acct-Terminate-Cause",      49,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.10  Acct-Terminate-Cause
   { "Acct-Multi-Session-Id",     50,   TRAD_STRING,   0 },              // RFC2866 Section 5.11  Acct-Multi-Session-Id
   { "Acct-Link-Count",           51,   TRAD_INTEGER,  0 },              // RFC2866 Section 5.12  Acct-Link-Count
   { "CHAP-Challenge",            60,   TRAD_OCTETS,   0 },              // RFC2865 Section 5.40  CHAP-Challenge
   { "NAS-Port-Type",             61,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.41  NAS-Port-Type
   { "Port-Limit",                62,   TRAD_INTEGER,  0 },              // RFC2865 Section 5.42  Port-Limit
   { "Login-LAT-Port",            63,   TRAD_STRING,   0 },              // RFC2865 Section 5.43  Login-LAT-Port
   { "Tunnel-Type",               64,   TRAD_INTEGER,  TRAD_HAS_TAG },   // RFC2868 Section 3.1   Tunnel-Type
   { "Tunnel-Medium-Type",        65,   TRAD_INTEGER,  TRAD_HAS_TAG },   // RFC2868 Section 3.2   Tunnel-Medium-Type
   { "Tunnel-Client-Endpoint",    66,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.3   Tunnel-Client-Endpoint
   { "Tunnel-Server-Endpoint",    67,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.4   Tunnel-Server-Endpoint
   { "Acct-Tunnel-Connection",    68,   TRAD_STRING,   0 },              // RFC2867 Section 4.1   Acct-Tunnel-Connection
   { "Tunnel-Password",           69,   TRAD_STRING,   TRAD_HAS_TAG|TRAD_ENCRYPT2 }, // RFC2868 Section 3.5 Tunnel-Password
   { "Tunnel-Private-Group-Id",   81,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.6   Tunnel-Private-Group-Id
   { "Tunnel-Assignment-Id",      82,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.7   Tunnel-Assignment-Id
   { "Tunnel-Preference",         83,   TRAD_INTEGER,  TRAD_HAS_TAG },   // RFC2868 Section 3.8   Tunnel-Preference
   { "Acct-Tunnel-Packets-Lost",  86,   TRAD_INTEGER,  0 },              // RFC2867 Section 4.2   Acct-Tunnel-Packets-Lost
   { "Tunnel-Client-Auth-Id",     90,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.9   Tunnel-Client-Auth-Id
   { "Tunnel-Server-Auth-Id",     91,   TRAD_STRING,   TRAD_HAS_TAG },   // RFC2868 Section 3.10  Tunnel-Server-Auth-Id
   { NULL, 0, 0, 0 }
};


static const struct
{
   const char *          attr_name;
   const char *          value_name;
   uint64_t              data;
} tinyrad_dict_default_values[] =
{
   { "Acct-Authentic",            "RADIUS",                    1 },  // RFC2866 Section 5.6 Acct-Authentic
   { "Acct-Authentic",            "Local",                     2 },  // RFC2866 Section 5.6 Acct-Authentic
   { "Acct-Authentic",            "Remote",                    3 },  // RFC2866 Section 5.6 Acct-Authentic
   { "Acct-Authentic",            "Diameter",                  4 },  // RFC2866 Section 5.6 Acct-Authentic

   { "Acct-Status-Type",          "Start",                     1 },  // RFC2866 Section 5.1 Acct-Status-Type
   { "Acct-Status-Type",          "Stop",                      2 },  // RFC2866 Section 5.1 Acct-Status-Type
   { "Acct-Status-Type",          "Interim-Update",            3 },  // RFC2866 Section 5.1 Acct-Status-Type
   { "Acct-Status-Type",          "Accounting-On",             7 },  // RFC2866 Section 5.1 Acct-Status-Type
   { "Acct-Status-Type",          "Accounting-Off",            8 },  // RFC2866 Section 5.1 Acct-Status-Type
   { "Acct-Status-Type",          "Tunnel-Start",              9 },  // RFC2867 Section 3.1 Tunnel-Start
   { "Acct-Status-Type",          "Tunnel-Stop",              10 },  // RFC2867 Section 3.2 Tunnel-Stop
   { "Acct-Status-Type",          "Tunnel-Reject",            11 },  // RFC2867 Section 3.3 Tunnel-Reject
   { "Acct-Status-Type",          "Tunnel-Link-Start",        12 },  // RFC2867 Section 3.4 Tunnel-Link-Start
   { "Acct-Status-Type",          "Tunnel-Link-Stop",         13 },  // RFC2867 Section 3.5 Tunnel-Link-Stop
   { "Acct-Status-Type",          "Tunnel-Link-Reject",       14 },  // RFC2867 Section 3.6 Tunnel-Link-Reject
   { "Acct-Status-Type",          "Failed",                   15 },  // RFC2866 Section 5.1 Acct-Status-Type

   { "Acct-Terminate-Cause",      "User-Request",              1 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Lost-Carrier",              2 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Lost-Service",              3 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Idle-Timeout",              4 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Session-Timeout",           5 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Admin-Reset",               6 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Admin-Reboot",              7 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Port-Error",                8 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "NAS-Error",                 9 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "NAS-Request",              10 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "NAS-Reboot",               11 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Port-Unneeded",            12 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Port-Preempted",           13 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Port-Suspended",           14 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Service-Unavailable",      15 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Callback",                 16 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "User-Error",               17 },  // RFC2866 Section 5.10 Acct-Terminate-Cause
   { "Acct-Terminate-Cause",      "Host-Request",             18 },  // RFC2866 Section 5.10 Acct-Terminate-Cause

   { "Framed-Compression",        "None",                      0 },  // RFC2865 Section 5.13 Framed-Compression
   { "Framed-Compression",        "Van-Jacobson-TCP-IP",       1 },  // RFC2865 Section 5.13 Framed-Compression
   { "Framed-Compression",        "IPX-Header-Compression",    2 },  // RFC2865 Section 5.13 Framed-Compression
   { "Framed-Compression",        "Stac-LZS",                  3 },  // RFC2865 Section 5.13 Framed-Compression

   { "Framed-Protocol",           "PPP",                       1 },  // RFC2865 Section 5.7 Framed-Protocol
   { "Framed-Protocol",           "SLIP",                      2 },  // RFC2865 Section 5.7 Framed-Protocol
   { "Framed-Protocol",           "ARAP",                      3 },  // RFC2865 Section 5.7 Framed-Protocol
   { "Framed-Protocol",           "Gandalf-SLML",              4 },  // RFC2865 Section 5.7 Framed-Protocol
   { "Framed-Protocol",           "Xylogics-IPX-SLIP",         5 },  // RFC2865 Section 5.7 Framed-Protocol
   { "Framed-Protocol",           "X.75-Synchronous",          6 },  // RFC2865 Section 5.7 Framed-Protocol

   { "Framed-Routing",            "None",                      0 },  // RFC2865 Section 5.10 Framed-Routing
   { "Framed-Routing",            "Broadcast",                 1 },  // RFC2865 Section 5.10 Framed-Routing
   { "Framed-Routing",            "Listen",                    2 },  // RFC2865 Section 5.10 Framed-Routing
   { "Framed-Routing",            "Broadcast-Listen",          3 },  // RFC2865 Section 5.10 Framed-Routing

   { "Login-Service",             "Telnet",                    0 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "Rlogin",                    1 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "TCP-Clear",                 2 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "PortMaster",                3 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "LAT",                       4 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "X25-PAD",                   5 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "X25-T3POS",                 6 },  // RFC2865 Section 5.15 Login-Service
   { "Login-Service",             "TCP-Clear-Quiet",           8 },  // RFC2865 Section 5.15 Login-Service

   { "Login-TCP-Port",            "Telnet",                   23 },  // RFC2865 Section 5.16 Login-TCP-Port
   { "Login-TCP-Port",            "Rlogin",                  513 },  // RFC2865 Section 5.16 Login-TCP-Port
   { "Login-TCP-Port",            "Rsh",                     514 },  // RFC2865 Section 5.16 Login-TCP-Port

   { "NAS-Port-Type",             "Async",                     0 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Sync",                      1 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "ISDN",                      2 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "ISDN-V120",                 3 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "ISDN-V110",                 4 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Virtual",                   5 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "PIAFS",                     6 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "HDLC-Clear-Channel",        7 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "X.25",                      8 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "X.75",                      9 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "G.3-Fax",                  10 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "SDSL",                     11 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "ADSL-CAP",                 12 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "ADSL-DMT",                 13 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "IDSL",                     14 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Ethernet",                 15 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "xDSL",                     16 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Cable",                    17 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Wireless-Other",           18 },  // RFC2865 Section 5.41 NAS-Port-Type
   { "NAS-Port-Type",             "Wireless-802.11",          19 },  // RFC2865 Section 5.41 NAS-Port-Type

   { "Service-Type",              "Login-User",                1 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Framed-User",               2 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Callback-Login-User",       3 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Callback-Framed-User",      4 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Outbound-User",             5 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Administrative-User",       6 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "NAS-Prompt-User",           7 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Authenticate-Only",         8 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Callback-NAS-Prompt",       9 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Call-Check",               10 },  // RFC2865 Section 5.6 Service-Type
   { "Service-Type",              "Callback-Administrative",  11 },  // RFC2865 Section 5.6 Service-Type

   { "Termination-Action",        "Default",                   0 },  // RFC2865 Section 5.29 Termination-Action
   { "Termination-Action",        "RADIUS-Request",            1 },  // RFC2865 Section 5.29 Termination-Action

   { "Tunnel-Medium-Type",        "IPv4",                      1 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "IPv6",                      2 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "NSAP",                      3 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "HDLC",                      4 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "BBN-1822",                  5 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "IEEE-802",                  6 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "E.163",                     7 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "E.164",                     8 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "F.69",                      9 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "X.121",                    10 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "IPX",                      11 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "Appletalk",                12 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "DecNet-IV",                13 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "Banyan-Vines",             14 },  // RFC2868 Section 3.2  Tunnel-Medium-Type
   { "Tunnel-Medium-Type",        "E.164-NSAP",               15 },  // RFC2868 Section 3.2  Tunnel-Medium-Type

   { "Tunnel-Type",               "PPTP",                      1 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "L2F",                       2 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "L2TP",                      3 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "ATMP",                      4 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "VTP",                       5 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "AH",                        6 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "IP",                        7 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "MIN-IP",                    8 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "ESP",                       9 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "GRE",                      10 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "DVS",                      11 },  // RFC2868 Section 3.1  Tunnel-Type
   { "Tunnel-Type",               "IP-in-IP",                 12 },  // RFC2868 Section 3.1  Tunnel-Type

   { NULL, NULL, 0}
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

/// Add directory to search path for dictionary files
///
/// @param[in]  dict          dictionary reference
/// @param[in]  path          string contain directory name
/// @return returns error code
int
tinyrad_dict_add_path(
         TinyRadDict *                 dict,
         const char *                  path )
{
   int             rc;
   size_t          size;
   char **         paths;
   struct stat     sb;

   assert(dict != NULL);
   assert(path != NULL);

   if ((rc = tinyrad_stat(path, &sb, S_IFDIR)) != TRAD_SUCCESS)
      return(rc);

   size = sizeof(char **) * (dict->paths_len + 2);
   if ((paths = realloc(dict->paths, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->paths = paths;

   if ((dict->paths[dict->paths_len] = strdup(path)) == NULL)
      return(TRAD_ENOMEM);
   dict->paths_len++;
   dict->paths[dict->paths_len] = NULL;

   return(TRAD_SUCCESS);
}


/// Destroys and frees resources of a RADIUS dictionary attribute
///
/// @param[out] attrp         reference to dictionary attribute
int
tinyrad_dict_attr_add(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor,
         TinyRadDictAttr **            attrp,
         const char *                  name,
         uint32_t                      type,
         uint8_t                       datatype,
         uint32_t                      flags )
{
   uint32_t             vendor_id;
   size_t               size;
   void *               ptr;
   TinyRadDictAttr *    attr;

   assert(dict      != NULL);
   assert(name      != NULL);
   assert(type      != 0);
   assert(datatype  != 0);

   vendor_id = ((vendor)) ? vendor->id : 0;

   // verify attribute doesn't exist
   if ((attr = tinyrad_dict_attr_lookup(dict, vendor_id, name, 0)) != NULL)
   {
      if ( (attr->flags|TRAD_DFLT_ATTR) != (flags|TRAD_DFLT_ATTR) )
         return(TRAD_EEXISTS);
      if (attr->type != type)
         return(TRAD_EEXISTS);
      if (attr->vendor_id != vendor_id)
         return(TRAD_EEXISTS);
      if (attr->data_type != datatype)
         return(TRAD_EEXISTS);
      if ((attrp))
         *attrp = attr;
      return(TRAD_SUCCESS);
   };
   if ((tinyrad_dict_attr_lookup(dict, vendor_id, NULL, type)))
      return(TRAD_EEXISTS);

   // resize attribute lists
   size = sizeof(TinyRadDictAttr *) * (dict->attrs_len+2);
   if ((ptr = realloc(dict->attrs, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->attrs = ptr;
   if ((vendor))
   {
      size = sizeof(TinyRadDictAttr *) * (vendor->attrs_len+2);
      if ((ptr = realloc(vendor->attrs, size)) == NULL)
         return(TRAD_ENOMEM);
      vendor->attrs = ptr;

      if ((ptr = realloc(vendor->attrs_type, size)) == NULL)
         return(TRAD_ENOMEM);
      vendor->attrs_type = ptr;
   } else {
      size = sizeof(TinyRadDictAttr *) * (dict->attrs_type_len+2);
      if ((ptr = realloc(dict->attrs_type, size)) == NULL)
         return(TRAD_ENOMEM);
      dict->attrs_type= ptr;
   };

   // allocate memory
   if ((attr = malloc(sizeof(TinyRadDictAttr))) == NULL)
      return(TRAD_ENOMEM);
   bzero(attr, sizeof(TinyRadDictAttr));
   attr->type      = type;
   attr->data_type = datatype;
   attr->flags     = flags;
   if ((vendor))
   {
      attr->vendor_id = vendor->id;
      attr->type_octs = vendor->type_octs;
      attr->len_octs  = vendor->len_octs;
   };
   if ((attr->name = strdup(name)) == NULL)
   {
      tinyrad_dict_attr_destroy(attr);
      return(TRAD_ENOMEM);
   };

   // save attribute
   dict->attrs[dict->attrs_len + 0] = attr;
   dict->attrs[dict->attrs_len + 1] = NULL;
   dict->attrs_len++;
   qsort(dict->attrs, dict->attrs_len, sizeof(TinyRadDictAttr *), tinyrad_dict_attr_cmp_name);
   if ((vendor))
   {
      vendor->attrs[      vendor->attrs_len + 0 ] = attr;
      vendor->attrs[      vendor->attrs_len + 1 ] = NULL;
      vendor->attrs_type[ vendor->attrs_len + 0 ] = attr;
      vendor->attrs_type[ vendor->attrs_len + 1 ] = NULL;
      vendor->attrs_len++;
      qsort(vendor->attrs,      vendor->attrs_len, sizeof(TinyRadDictAttr *), tinyrad_dict_attr_cmp_name);
      qsort(vendor->attrs_type, vendor->attrs_len, sizeof(TinyRadDictAttr *), tinyrad_dict_attr_cmp_type);
   } else {
      dict->attrs_type[dict->attrs_type_len + 0] = attr;
      dict->attrs_type[dict->attrs_type_len + 1] = NULL;
      dict->attrs_type_len++;
      qsort(dict->attrs_type, dict->attrs_type_len, sizeof(TinyRadDictAttr *), tinyrad_dict_attr_cmp_type);
   }
   if ((attrp))
      *attrp = attr;

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_attr_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictAttr * attr1;
   const TinyRadDictAttr * attr2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   attr1 = *((const TinyRadDictAttr * const *)ptr1);
   attr2 = *((const TinyRadDictAttr * const *)ptr2);

   return(strcasecmp(attr1->name, attr2->name));
}


int
tinyrad_dict_attr_cmp_type(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictAttr *  attr1;
   const TinyRadDictAttr *  attr2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   attr1 = *((const TinyRadDictAttr * const *)ptr1);
   attr2 = *((const TinyRadDictAttr * const *)ptr2);

   return( (int)(((int64_t)attr1->type) - ((int64_t)attr2->type)) );
}


/// Destroys and frees resources of a RADIUS dictionary attribute
///
/// @param[in]  attr          reference to dictionary attribute
void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr )
{
   if (!(attr))
      return;
   if ((attr->name))
      free(attr->name);
   bzero(attr, sizeof(TinyRadDictAttr));
   free(attr);
   return;
}


TinyRadDictAttr *
tinyrad_dict_attr_lookup(
         TinyRadDict *                dict,
         uint32_t                     vendor_id,
         const char *                 name,
         uint32_t                     type )
{
   TinyRadDictVendor *  vendor;
   size_t               len;
   void **              list;
   const void *         idx;
   int (*compar)(const void *, const void *);

   assert(dict   != NULL);

   if ((name))
   {
      list   = (void **)dict->attrs;
      len    = dict->attrs_len;
      idx    = name;
      compar = tinyrad_dict_attr_lookup_name;
   } else {
      list   = (void **)dict->attrs_type;
      len    = dict->attrs_len;
      if ((vendor_id))
      {
         if ((vendor = tinyrad_dict_vendor_lookup(dict, NULL, vendor_id)) == NULL)
            return(NULL);
         list = (void **)vendor->attrs_type;
         len  = vendor->attrs_len;
      };
      idx    = &type;
      compar = tinyrad_dict_attr_lookup_type;
   };

   return(tinyrad_dict_lookup(list, len, idx, compar));
}


int
tinyrad_dict_attr_lookup_name(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictVendor *  vendor = data;
   const char *               name   = idx;
   return(strcasecmp(name, vendor->name));
}


int
tinyrad_dict_attr_lookup_type(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictAttr *    attr = data;
   uint32_t                   type = *((const uint32_t *)idx);
   if (attr->type == type)
      return(0);
   if (attr->type < type)
      return(-1);
   return(1);
}


_TINYRAD_F int
tinyrad_dict_defaults(
         TinyRadDict *                 dict,
         char ***                      msgsp,
         uint32_t                      opts )
{
   int                rc;
   size_t             pos;
   uint32_t           type;
   uint8_t            datatype;
   uint32_t           flags;
   uint64_t           data;
   const char *       attr_name;
   const char *       value_name;
   TinyRadDictAttr *  attr;

   assert(dict != NULL);
   assert(opts == 0);

   if ((msgsp))
      *msgsp = NULL;

   for(pos = 0; ((tinyrad_dict_default_attrs[pos].name)); pos++)
   {
      attr_name = tinyrad_dict_default_attrs[pos].name;
      type      = (uint32_t)tinyrad_dict_default_attrs[pos].type;
      datatype  = (uint8_t)tinyrad_dict_default_attrs[pos].data_type;
      flags     = (uint32_t)tinyrad_dict_default_attrs[pos].flags;
      flags    |= TRAD_DFLT_ATTR;
      if ((rc = tinyrad_dict_attr_add(dict, NULL, NULL, attr_name, type, datatype, flags)) != TRAD_SUCCESS)
         return(tinyrad_error_msgs(rc, msgsp, "default attribute %s(%" PRIu32 "): ", attr_name, type));
   };

   attr = NULL;
   for(pos = 0; ((tinyrad_dict_default_values[pos].attr_name)); pos++)
   {
      attr_name  = tinyrad_dict_default_values[pos].attr_name;
      value_name = tinyrad_dict_default_values[pos].value_name;
      data       = tinyrad_dict_default_values[pos].data;
      if ((attr))
         if ((strcasecmp(attr_name, attr->name)))
            attr = NULL;
      if (!(attr))
         attr = tinyrad_dict_attr_lookup(dict, 0, attr_name, 0);
      if (!(attr))
         return(tinyrad_error_msgs(TRAD_ENOENT, msgsp, "default value: %s %s(%" PRIu64 "): ", attr_name, value_name, data));
      if ((rc = tinyrad_dict_value_add(attr, NULL, value_name, data)) != TRAD_SUCCESS)
         return(tinyrad_error_msgs(rc, msgsp, "default value: %s %s(%" PRIu64 "): ", attr_name, value_name, data));
   };

   return(TRAD_SUCCESS);
}


/// Initialize dicitionary file buffer
/// 
/// @param[in]  dict          dictionary reference
void
tinyrad_dict_destroy(
         TinyRadDict *                dict )
{
   size_t        pos;

   if (!(dict))
      return;

   if (atomic_fetch_sub(&dict->ref_count, 1) > 1)
      return;

   // free attributes
   if ((dict->attrs))
   {
      for(pos = 0; (pos < dict->attrs_len); pos++)
         tinyrad_dict_attr_destroy(dict->attrs[pos]);
      free(dict->attrs);
   };
   if ((dict->attrs_type))
      free(dict->attrs_type);

   // free paths
   if ((dict->paths))
   {
      for(pos = 0; (pos < dict->paths_len); pos++)
         free(dict->paths[pos]);
      free(dict->paths);
   };

   // free vendors
   if ((dict->vendors))
   {
      for(pos = 0; (pos < dict->vendors_len); pos++)
         tinyrad_dict_vendor_destroy(dict->vendors[pos]);
      free(dict->vendors);
   };
   if ((dict->vendors_id))
      free(dict->vendors_id);

   bzero(dict, sizeof(TinyRadDict));
   free(dict);

   return;
}


/// Imports file into dictionary
///
/// @param[in]  dict          dictionary reference
/// @param[in]  path          path to import file
/// @param[out] msgsp         pointer to generated error details
/// @param[in]  opts          import options
/// @return returns error code
int
tinyrad_dict_import(
         TinyRadDict *                dict,
         const char *                 path,
         char ***                     msgsp,
         uint32_t                     opts )
{
   int                   rc;
   TinyRadFile *         file;
   TinyRadFile *         parent;
   TinyRadDictVendor *   vendor;

   assert(dict != NULL);
   assert(path != NULL);

   if ((msgsp))
      *msgsp = NULL;

   vendor = NULL;

   // initialize file buffer
   if ((rc = tinyrad_file_init(&file, path, dict->paths, NULL)) != TRAD_SUCCESS)
   {
      tinyrad_file_error(file, rc, msgsp);
      tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
      return(rc);
   };

   // loops through dictionary file
   while((file))
   {
      // reads next line
      if ((rc = tinyrad_file_readline(file, opts)) != TRAD_SUCCESS)
      {
         tinyrad_file_error(file, rc, msgsp);
         tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
         return(rc);
      };
      if (file->argc < 1)
      {
         parent = file->parent;
         tinyrad_file_destroy(file, TRAD_FILE_NORECURSE);
         file = parent;
         continue;
      };

      // perform requested action
      switch(tinyrad_map_lookup_name(tinyrad_dict_options, file->argv[0], NULL))
      {
         case TRAD_DICT_ATTRIBUTE:
         if ((rc = tinyrad_dict_import_attribute(dict, vendor, file, opts)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_BEGIN_VENDOR:
         if ((rc = tinyrad_dict_import_begin_vendor(dict, &vendor, file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_END_VENDOR:
         if ((rc = tinyrad_dict_import_end_vendor(dict, &vendor, file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_INCLUDE:
         if ((rc = tinyrad_dict_import_include(dict, &file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_VALUE:
         if ((rc = tinyrad_dict_import_value(dict, file, opts)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_VENDOR:
         if ((rc = tinyrad_dict_import_vendor(dict, file, opts)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         default:
         tinyrad_file_error(file, TRAD_ESYNTAX, msgsp);
         tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
         return(TRAD_ESYNTAX);
      };
   };

   tinyrad_file_error(NULL, TRAD_SUCCESS, msgsp);

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_import_attribute(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   uint8_t      datatype;
   uint32_t    type;
   uint32_t    flags;
   uint32_t    flag;
   int         rc;
   char *      ptr;
   char *      str;

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if ( (file->argc < 4) || (file->argc > 5) )
      return(TRAD_ESYNTAX);
   if ((datatype = (uint8_t)tinyrad_map_lookup_name(tinyrad_dict_data_type, file->argv[3], NULL)) == 0)
      return(TRAD_ESYNTAX);
   if ((type = (uint32_t)strtoull(file->argv[2], &ptr, 10)) == 0)
      return(TRAD_ESYNTAX);
   if ((ptr[0] != '\0') || (file->argv[2] == ptr))
      return(TRAD_ESYNTAX);

   flags = 0;
   if (file->argc == 5)
   {
      ptr = file->argv[4];
      while ((str = ptr) != NULL)
      {
         if ((ptr = index(str, ',')) != NULL)
            ptr[0] = '\0';
         if ((flag = (uint32_t)tinyrad_map_lookup_name(tinyrad_dict_attr_flags, str, NULL)) == 0)
            return(TRAD_ESYNTAX);
         if ( ((flag & TRAD_ENCRYPT_MASK) != 0) && ((flags & TRAD_ENCRYPT_MASK) != 0) )
            return(TRAD_ESYNTAX);
         flags |= flag;
         ptr = ((ptr)) ? &ptr[1] : NULL;
      };
   };

   if ((rc = tinyrad_dict_attr_add(dict, vendor, NULL, file->argv[1], type, datatype, flags)) != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_import_begin_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file )
{
   assert(dict    != NULL);
   assert(vendorp != NULL);

   if (file->argc != 2)
      return(TRAD_ESYNTAX);
   if ((*vendorp))
      return(TRAD_ESYNTAX);
   if ((*vendorp = tinyrad_dict_vendor_lookup(dict, file->argv[1], 0)) == NULL)
      return(TRAD_ESYNTAX);

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_import_end_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file )
{
   assert(dict    != NULL);
   assert(vendorp != NULL);

   if (file->argc != 2)
      return(TRAD_ESYNTAX);
   if (!(*vendorp))
      return(TRAD_ESYNTAX);
   if ((strcasecmp((*vendorp)->name, file->argv[1])))
      return(TRAD_ESYNTAX);

   *vendorp = NULL;

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_import_include(
         TinyRadDict *                dict,
         TinyRadFile **               filep )
{
   int            rc;
   TinyRadFile *  incl;

   assert(dict  != NULL);
   assert(filep != NULL);

   if ((*filep)->argc != 2)
      return(TRAD_ESYNTAX);

   if ((rc = tinyrad_file_init(&incl, (*filep)->argv[1], dict->paths, *filep)) != TRAD_SUCCESS)
   {
      *filep = ((incl)) ? incl : *filep;
      return(rc);
   };

   *filep = incl;

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_import_value(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   TinyRadDictAttr *  attr;
   uint64_t           number;
   int                rc;
   char *             ptr;

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if (file->argc == 3)
      return(TRAD_ESYNTAX);
   if ((attr = tinyrad_dict_attr_lookup(dict, 0, file->argv[1], 0)) == NULL)
      return(TRAD_ESYNTAX);
   number = (uint64_t)strtoull(file->argv[3], &ptr, 0);
   if ((ptr[0] != '\0') || (file->argv[3] == ptr))
      return(TRAD_ESYNTAX);

   if ((rc = tinyrad_dict_value_add(attr, NULL, file->argv[2], number)) != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
}


/// Imports file into dictionary
///
/// @param[in]  dict          dictionary reference
/// @param[in]  file          path to import file
/// @param[in]  opts          import options
/// @return returns error code
int
tinyrad_dict_import_vendor(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   int                     rc;
   char *                  endptr;
   char *                  str;
   uint32_t                id;
   uint32_t                type_octs;
   uint32_t                len_octs;

   TinyRadDictVendor *     vendor;

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   // checks arguments
   if ((file->argc < 3) || (file->argc > 4))
      return(TRAD_ESYNTAX);

   // initializes flags
   type_octs = 1;
   len_octs  = 1;

   // verifies arguments list length
   if ((file->argc < 3) || (file->argc > 4))
      return(TRAD_ESYNTAX);

   // process vendor-id
   id = (uint32_t)strtoul(file->argv[2], &endptr, 10);
   if (endptr[0] != '\0')
      return(TRAD_ESYNTAX);

   // process flags
   if (file->argc == 4)
   {
      // initial string checks
      if (!(strcmp("format=", file->argv[3])))
         return(TRAD_ESYNTAX);
      str = index(file->argv[3], '=');
      str++;

      // parse type length
      type_octs = (uint32_t)strtoul(str, &endptr, 10);
      if (endptr == str)
         return(TRAD_ESYNTAX);
      if (endptr[0] != ',')
         return(TRAD_ESYNTAX);
      str = &endptr[1];
      if ( (type_octs != 1) && (type_octs != 2) && (type_octs != 4) )
         return(TRAD_ESYNTAX);

      // parse value length
      len_octs = (uint32_t)strtoul(str, &endptr, 10);
      if (endptr == str)
         return(TRAD_ESYNTAX);
      if (endptr[0] != '\0')
         return(TRAD_ESYNTAX);
      if (len_octs > 2)
         return(TRAD_ESYNTAX);
   };

   // initialize vendor struct
   if ((rc = tinyrad_dict_vendor_add(dict, &vendor, file->argv[1], id, (uint8_t)type_octs, (uint8_t)len_octs)) != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
}


/// Initializes dictionary
///
/// @param[out] dictp         pointer to dictionary reference
/// @param[in]  opts          dictionary options
/// @return returns error code
int
tinyrad_dict_initialize(
         TinyRadDict **               dictp,
         uint32_t                     opts )
{
   TinyRadDict *    dict;

   assert(dictp != NULL);

   if (!(dict = malloc(sizeof(TinyRadDict))))
      return(-1);
   bzero(dict, sizeof(TinyRadDict));
   dict->opts      = opts;
   dict->ref_count = 1;

   // initializes paths
   if ((dict->paths = malloc(sizeof(char *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->paths[0] = NULL;

   // intializes vendors
   if ((dict->vendors = malloc(sizeof(TinyRadDictVendor *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->vendors[0] = NULL;
   if ((dict->vendors_id = malloc(sizeof(TinyRadDictVendor *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->vendors_id[0] = NULL;

   // initializes attributes
   if ((dict->attrs = malloc(sizeof(TinyRadDictAttr *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->attrs[0] = NULL;
   if ((dict->attrs_type = malloc(sizeof(TinyRadDictAttr *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->attrs_type[0] = NULL;

   *dictp = dict;

   return(TRAD_SUCCESS);
}


void *
tinyrad_dict_lookup(
         void **                      list,
         size_t                       len,
         const void *                 idx,
         int (*compar)(const void *, const void *) )
{
   int         rc;
   ssize_t     low;
   ssize_t     mid;
   ssize_t     high;

   assert(idx  != NULL);

   if (!(list))
      return(NULL);
   if (!(len))
      return(NULL);

   low  = 0;
   high = (ssize_t)(len - 1);
   mid  = (ssize_t)((len - 1) / 2);

   while(mid > low)
   {
      if ((rc = (*compar)(list[mid], idx)) == 0)
         return(list[mid]);
      else if (rc < 0)
         high = mid - 1;
      else
         low = mid;
      mid = (high + low) / 2;
   };

   if ((*compar)(list[mid], idx) == 0)
      return(list[mid]);
   if ((*compar)(list[high], idx) == 0)
      return(list[high]);

   return(NULL);
}


void
tinyrad_dict_print(
         TinyRadDict *                 dict,
         uint32_t                      opts )
{
   size_t     pos;

   assert(dict   != NULL);
   assert(opts   != 0);

   printf("# processed dictionary\n");

   for(pos = 0; (pos < dict->attrs_type_len); pos ++)
      if (!(dict->attrs_type[pos]->vendor_id))
         tinyrad_dict_print_attribute(dict, dict->attrs_type[pos]);

   for(pos = 0; (pos < dict->vendors_len); pos++)
      tinyrad_dict_print_vendor(dict, dict->vendors[pos]);

   printf("# end of processed dictionary\n");

   return;
}


void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr )
{
   size_t              pos;
   uint32_t            flags;
   TinyRadDictValue *  value;

   assert(dict != NULL);
   assert(attr != NULL);

   printf("ATTRIBUTE %s ", attr->name);
   printf("%" PRIu32 " ", attr->type);
   printf("%s", tinyrad_map_lookup_value(tinyrad_dict_data_type, attr->data_type, NULL));

   flags = 0;
   for(pos = 0; ((tinyrad_dict_attr_flags[pos].name)); pos++)
   {
      if ((attr->flags & tinyrad_dict_attr_flags[pos].value) != 0)
      {
         printf("%c%s", ((!(flags)) ? ' ' : ','), tinyrad_dict_attr_flags[pos].name);
         flags++;
      };
   };
   printf("\n");

   for(pos = 0; (pos < attr->values_len); pos++)
   {
      value = attr->values_numeric[pos];
      printf("VALUE %s %s %" PRIu64 "\n", attr->name, value->name, value->value );
   };

   return;
}


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor )
{
   size_t pos;

   assert(dict   != NULL);
   assert(vendor != NULL);

   printf("VENDOR %s ", vendor->name);
   printf("%" PRIu32, vendor->id);
   if ( (vendor->len_octs != 1) || (vendor->type_octs != 1) )
      printf(" format=%" PRIu8 ",%" PRIu8, vendor->type_octs, vendor->len_octs);
   printf("\n");

   if (!(vendor->attrs_len))
      return;

   printf("BEGIN-VENDOR %s\n", vendor->name);
   for(pos = 0; (pos < vendor->attrs_len); pos++)
      tinyrad_dict_print_attribute(dict, vendor->attrs_type[pos]);
   printf("END-VENDOR %s\n", vendor->name);

   return;
}


int
tinyrad_dict_value_add(
         TinyRadDictAttr *             attr,
         TinyRadDictValue **           valuep,
         const char *                  name,
         uint64_t                      numeral )
{
   TinyRadDictValue *   value;
   size_t               size;
   void *               ptr;

   assert(attr      != NULL);
   assert(name      != NULL);

   // verify value doesn't exist
   if ((value = tinyrad_dict_value_lookup(attr, name, 0)) != NULL)
   {
      if (numeral != value->value)
         return(TRAD_EEXISTS);
      if ((valuep))
         *valuep = value;
      return(TRAD_SUCCESS);
   };
   if ((tinyrad_dict_value_lookup(attr, NULL, numeral)))
      return(TRAD_EEXISTS);

   // resize attribute lists
   size = sizeof(TinyRadDictValue *) * (attr->values_len+2);
   if ((ptr = realloc(attr->values_name, size)) == NULL)
      return(TRAD_ENOMEM);
   attr->values_name = ptr;
   if ((ptr = realloc(attr->values_numeric, size)) == NULL)
      return(TRAD_ENOMEM);
   attr->values_numeric = ptr;

   // allocate memory
   if ((value = malloc(sizeof(TinyRadDictValue))) == NULL)
      return(TRAD_ENOMEM);
   bzero(value, sizeof(TinyRadDictValue));
   value->value = numeral;
   if ((value->name = strdup(name)) == NULL)
   {
      tinyrad_dict_value_destroy(value);
      return(TRAD_ENOMEM);
   };

   // save attribute
   attr->values_name[    attr->values_len + 0 ] = value;
   attr->values_name[    attr->values_len + 1 ] = NULL;
   attr->values_numeric[ attr->values_len + 0 ] = value;
   attr->values_numeric[ attr->values_len + 1 ] = NULL;
   attr->values_len++;
   qsort(attr->values_name,    attr->values_len, sizeof(TinyRadDictValue *), tinyrad_dict_value_cmp_name);
   qsort(attr->values_numeric, attr->values_len, sizeof(TinyRadDictValue *), tinyrad_dict_value_cmp_numeric);
   if ((valuep))
      *valuep = value;

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_value_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictValue * value1;
   const TinyRadDictValue * value2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   value1 = *((const TinyRadDictValue * const *)ptr1);
   value2 = *((const TinyRadDictValue * const *)ptr2);

   return(strcasecmp(value1->name, value2->name));
}


int
tinyrad_dict_value_cmp_numeric(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictValue * value1;
   const TinyRadDictValue * value2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   value1 = *((const TinyRadDictValue * const *)ptr1);
   value2 = *((const TinyRadDictValue * const *)ptr2);

   return( (int)(((int64_t)value1->value) - ((int64_t)value2->value)) );
}


void
tinyrad_dict_value_destroy(
         TinyRadDictValue *           value )
{
   if (!(value))
      return;

   if ((value->name))
      free(value->name);

   bzero(value, sizeof(TinyRadDictValue));
   free(value);

   return;
}


TinyRadDictValue *
tinyrad_dict_value_lookup(
         TinyRadDictAttr *            attr,
         const char *                 name,
         uint64_t                     num )
{
   void **         list;
   const void *    idx;
   int (*compar)(const void *, const void *);

   assert(attr   != NULL);

   if ((name))
   {
      list   = (void **)attr->values_name;
      idx    = name;
      compar = tinyrad_dict_value_lookup_name;
   } else {
      list   = (void **)attr->values_numeric;
      idx    = &num;
      compar = tinyrad_dict_value_lookup_numeric;
   };

   return(tinyrad_dict_lookup(list, attr->values_len, idx, compar));
}


int
tinyrad_dict_value_lookup_name(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictValue *  value;
   const char *              name;
   value = data;
   name  = idx;
   return(strcasecmp(name, value->name));
}


int
tinyrad_dict_value_lookup_numeric(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictValue *  value;
   uint64_t                  val;
   value = data;
   val   = *((const uint64_t *)idx);
   if (value->value == val)
      return(0);
   return( (value->value < val) ? -1 : 1 );
}


/// wrapper around stat() for dictionary processing
///
/// @param[out] vendorp       dictionar vendor reference
int
tinyrad_dict_vendor_add(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         const char *                 name,
         uint32_t                     id,
         uint8_t                      type_octs,
         uint8_t                      len_octs )
{
   size_t               size;
   void *               ptr;
   TinyRadDictVendor *  vendor;

   assert(dict   != NULL);
   assert(name   != NULL);

   // verify attribute doesn't exist
   if ((vendor = tinyrad_dict_vendor_lookup(dict, name, 0)) != NULL)
   {
      if (vendor->id != id)
         return(TRAD_EEXISTS);
      if (vendor->type_octs != type_octs)
         return(TRAD_EEXISTS);
      if (vendor->len_octs != len_octs)
         return(TRAD_EEXISTS);
      if ((vendorp))
         *vendorp = vendor;
      return(TRAD_SUCCESS);
   };
   if ((tinyrad_dict_vendor_lookup(dict, NULL, id)))
      return(TRAD_EEXISTS);

   // resize vendor lists
   size = sizeof(TinyRadDictVendor *) * (dict->vendors_len + 2);
   if ((ptr = realloc(dict->vendors, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors = ptr;
   if ((ptr = realloc(dict->vendors_id, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors_id = ptr;

   // initialize vendor
   if ((vendor = malloc(sizeof(TinyRadDictVendor))) == NULL)
      return(TRAD_ENOMEM);
   bzero(vendor, sizeof(TinyRadDictVendor));
   vendor->id        = id;
   vendor->type_octs = type_octs;
   vendor->len_octs  = len_octs;
   vendor->ref_count = 1;

   // copy vendor name
   if ((vendor->name = strdup(name)) == NULL)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return(TRAD_ENOMEM);
   };

   // initialize vendor attribute lists
   if ((vendor->attrs = malloc(sizeof(TinyRadDictAttr *))) == NULL)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return(TRAD_ENOMEM);
   };
   vendor->attrs[0] = NULL;
   if ((vendor->attrs_type = malloc(sizeof(TinyRadDictAttr *))) == NULL)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return(TRAD_ENOMEM);
   };
   vendor->attrs_type[0] = NULL;

   // save vendor
   dict->vendors[    dict->vendors_len ] = vendor;
   dict->vendors_id[ dict->vendors_len ] = vendor;
   dict->vendors_len++;
   dict->vendors[    dict->vendors_len ] = NULL;
   dict->vendors_id[ dict->vendors_len ] = NULL;
   if ((vendorp))
      *vendorp = vendor;

   // sort vendors
   qsort(dict->vendors,    dict->vendors_len, sizeof(TinyRadDictVendor *), tinyrad_dict_vendor_cmp_name);
   qsort(dict->vendors_id, dict->vendors_len, sizeof(TinyRadDictVendor *), tinyrad_dict_vendor_cmp_id);

   return(0);
}


/// wrapper around stat() for dictionary processing
///
int
tinyrad_dict_vendor_cmp_id(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictVendor *  vendor1;
   const TinyRadDictVendor *  vendor2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   vendor1 = *((const TinyRadDictVendor * const *)ptr1);
   vendor2 = *((const TinyRadDictVendor * const *)ptr2);

   return( (int)(((int64_t)vendor1->id) - ((int64_t)vendor2->id)) );
}


/// wrapper around stat() for dictionary processing
///
int
tinyrad_dict_vendor_cmp_name(
         const void *                 ptr1,
         const void *                 ptr2 )
{
   const TinyRadDictVendor * vendor1;
   const TinyRadDictVendor * vendor2;

   assert(ptr1 != NULL);
   assert(ptr2 != NULL);

   vendor1 = *((const TinyRadDictVendor * const *)ptr1);
   vendor2 = *((const TinyRadDictVendor * const *)ptr2);

   return(strcasecmp(vendor1->name, vendor2->name));
}


/// wrapper around stat() for dictionary processing
///
/// @param[in]  vendor        dictionar vendor reference
void
tinyrad_dict_vendor_destroy(
         TinyRadDictVendor *          vendor )
{
   if (!(vendor))
      return;

   if (atomic_fetch_sub(&vendor->ref_count, 1) > 1)
      return;

   if ((vendor->name))
      free(vendor->name);

   if ((vendor->attrs))
      free(vendor->attrs);
   if ((vendor->attrs_type))
      free(vendor->attrs_type);

   bzero(vendor, sizeof(TinyRadDictVendor));
   free(vendor);

   return;
}


/// wrapper around stat() for dictionary processing
///
TinyRadDictVendor *
tinyrad_dict_vendor_lookup(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     id )
{
   void **         list;
   const void *    idx;
   int (*compar)(const void *, const void *);

   assert(dict   != NULL);

   if ((name))
   {
      list   = (void **)dict->vendors;
      idx    = name;
      compar = tinyrad_dict_vendor_lookup_name;
   } else {
      list   = (void **)dict->vendors_id;
      idx    = &id;
      compar = tinyrad_dict_vendor_lookup_id;
   };

   return(tinyrad_dict_lookup(list, dict->vendors_len, idx, compar));
}


/// wrapper around stat() for dictionary processing
///
int
tinyrad_dict_vendor_lookup_id(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictVendor *  vendor;
   uint32_t                   id;
   vendor = data;
   id     = *((const uint32_t *)idx);
   if (vendor->id == id)
      return(0);
   return( (vendor->id < id) ? -1 : 1 );
}


/// wrapper around stat() for dictionary processing
///
int
tinyrad_dict_vendor_lookup_name(
         const void *                 data,
         const void *                 idx )
{
   const TinyRadDictVendor *  vendor;
   const char *               name;
   vendor = data;
   name   = idx;
   return(strcasecmp(name, vendor->name));
}

/* end of source */
