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
#define _LIB_LIBTINYRAD_LDATA_C 1
#include "ldata.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stddef.h>
#include <sys/types.h>


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_dict_default_vendors[]
const TinyRadDictVendorDef tinyrad_dict_default_vendors[] =
{
   { NULL, 0, 0, 0 }
};


#pragma mark tinyrad_dict_default_attrs[]
// RADIUS attributes: https://www.iana.org/assignments/radius-types/radius-types.xhtml
const TinyRadDictAttrDef tinyrad_dict_default_attrs[] =
{
   // Name                      Type  Vendor ID  Vendor Type      Data Type            Flags
   { "User-Name",                  1,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.1  User-Name
   { "User-Password",              2,         0,           0,   TRAD_TEXT,     TRAD_ENCRYPT1 },  // RFC2865 Section 5.2  User-Password
   { "CHAP-Password",              3,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.3   CHAP-Password
   { "NAS-IP-Address",             4,         0,           0,   TRAD_IPV4ADDR,             0 },  // RFC2865 Section 5.4   NAS-IP-Address
   { "NAS-Port",                   5,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.5   NAS-Port
   { "Service-Type",               6,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.6   Service-Type
   { "Framed-Protocol",            7,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.7   Framed-Protocol
   { "Framed-IP-Address",          8,         0,           0,   TRAD_IPV4ADDR,             0 },  // RFC2865 Section 5.8   Framed-IP-Address
   { "Framed-IP-Netmask",          9,         0,           0,   TRAD_IPV4ADDR,             0 },  // RFC2865 Section 5.9   Framed-IP-Netmask
   { "Framed-Routing",            10,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.10  Framed-Routing
   { "Filter-Id",                 11,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.11  Filter-Id
   { "Framed-MTU",                12,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.12  Framed-MTU
   { "Framed-Compression",        13,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.13  Framed-Compression
   { "Login-IP-Host",             14,         0,           0,   TRAD_IPV4ADDR,             0 },  // RFC2865 Section 5.14  Login-IP-Host
   { "Login-Service",             15,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.15  Login-Service
   { "Login-TCP-Port",            16,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.16  Login-TCP-Port
   // unassigned                  17
   { "Reply-Message",             18,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.18  Reply-Message
   { "Callback-Number",           19,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.19  Callback-Number
   { "Callback-Id",               20,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.20  Callback-Id
   // unassigned                  21
   { "Framed-Route",              22,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.22  Framed-Route
   { "Framed-IPX-Network",        23,         0,           0,   TRAD_IPV4ADDR,             0 },  // RFC2865 Section 5.23  Framed-IPX-Network
   { "State",                     24,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.24  State
   { "Class",                     25,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.25  Class
   { "Vendor-Specific",           26,         0,           0,   TRAD_VSA,                  0 },  // RFC2865 Section 5.26  Vendor-Specific
   { "Session-Timeout",           27,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.27  Session-Timeout
   { "Idle-Timeout",              28,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.28  Idle-Timeout
   { "Termination-Action",        29,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.29  Termination-Action
   { "Called-Station-Id",         30,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.30  Called-Station-Id
   { "Calling-Station-Id",        31,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.31  Calling-Station-Id
   { "NAS-Identifier",            32,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.32  NAS-Identifier
   { "Proxy-State",               33,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.33  Proxy-State
   { "Login-LAT-Service",         34,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.34  Login-LAT-Service
   { "Login-LAT-Node",            35,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.35  Login-LAT-Node
   { "Login-LAT-Group",           36,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.36  Login-LAT-Group
   { "Framed-AppleTalk-Link",     37,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.37  Framed-AppleTalk-Link
   { "Framed-AppleTalk-Network",  38,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.38  Framed-AppleTalk-Network
   { "Framed-AppleTalk-Zone",     39,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.39  Framed-AppleTalk-Zone
   { "Acct-Status-Type",          40,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.1   Acct-Status-Type
   { "Acct-Delay-Time",           41,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.2   Acct-Delay-Time
   { "Acct-Input-Octets",         42,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.3   Acct-Input-Octets
   { "Acct-Output-Octets",        43,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.4   Acct-Output-Octets
   { "Acct-Session-Id",           44,         0,           0,   TRAD_TEXT,                 0 },  // RFC2866 Section 5.5   Acct-Session-Id
   { "Acct-Authentic",            45,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.6   Acct-Authentic
   { "Acct-Session-Time",         46,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.7   Acct-Session-Time
   { "Acct-Input-Packets",        47,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.8   Acct-Input-Packets
   { "Acct-Output-Packets",       48,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.9   Acct-Output-Packets
   { "Acct-Terminate-Cause",      49,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.10  Acct-Terminate-Cause
   { "Acct-Multi-Session-Id",     50,         0,           0,   TRAD_TEXT,                 0 },  // RFC2866 Section 5.11  Acct-Multi-Session-Id
   { "Acct-Link-Count",           51,         0,           0,   TRAD_INTEGER,              0 },  // RFC2866 Section 5.12  Acct-Link-Count
   { "Acct-Input-Gigawords",      52,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.1   Acct-Input-Gigawords
   { "Acct-Output-Gigawords",     53,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.2   Acct-Output-Gigawords
   // unassigned                  54
   { "Event-Timestamp",           55,         0,           0,   TRAD_TIME,                 0 },  // RFC2869 Section 5.3   Event-Timestamp
   { "Egress-VLANID",             56,         0,           0,   TRAD_INTEGER,              0 },  // RFC4675 Section 2.1   Egress-VLANID
   { "Ingress-Filters",           57,         0,           0,   TRAD_INTEGER,              0 },  // RFC4675 Section 2.2   Ingress-Filters
   { "Egress-VLAN-Name",          58,         0,           0,   TRAD_TEXT,                 0 },  // RFC4675 Section 2.3   Egress-VLAN-Name
   { "User-Priority-Table",       59,         0,           0,   TRAD_STRING,               0 },  // RFC4675 Section 2.4   User-Priority-Table
   { "CHAP-Challenge",            60,         0,           0,   TRAD_STRING,               0 },  // RFC2865 Section 5.40  CHAP-Challenge
   { "NAS-Port-Type",             61,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.41  NAS-Port-Type
   { "Port-Limit",                62,         0,           0,   TRAD_INTEGER,              0 },  // RFC2865 Section 5.42  Port-Limit
   { "Login-LAT-Port",            63,         0,           0,   TRAD_TEXT,                 0 },  // RFC2865 Section 5.43  Login-LAT-Port
   { "Tunnel-Type",               64,         0,           0,   TRAD_INTEGER,   TRAD_HAS_TAG },  // RFC2868 Section 3.1   Tunnel-Type
   { "Tunnel-Medium-Type",        65,         0,           0,   TRAD_INTEGER,   TRAD_HAS_TAG },  // RFC2868 Section 3.2   Tunnel-Medium-Type
   { "Tunnel-Client-Endpoint",    66,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.3   Tunnel-Client-Endpoint
   { "Tunnel-Server-Endpoint",    67,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.4   Tunnel-Server-Endpoint
   { "Acct-Tunnel-Connection",    68,         0,           0,   TRAD_TEXT,                 0 },  // RFC2867 Section 4.1   Acct-Tunnel-Connection
   { "Tunnel-Password",           69,         0,           0,   TRAD_TEXT,     TRAD_HAS_TAG|TRAD_ENCRYPT2 }, // RFC2868 Section 3.5 Tunnel-Password
   { "ARAP-Password",             70,         0,           0,   TRAD_STRING,               0 },  // RFC2869 Section 5.4   ARAP-Password
   { "ARAP-Features",             71,         0,           0,   TRAD_STRING,               0 },  // RFC2869 Section 5.5   ARAP-Features
   { "ARAP-Zone-Access",          72,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.6   ARAP-Zone-Access
   { "ARAP-Security",             73,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.7   ARAP-Security
   { "ARAP-Security-Data",        74,         0,           0,   TRAD_TEXT,                 0 },  // RFC2869 Section 5.8   ARAP-Security-Data
   { "Password-Retry",            75,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.9   Password-Retry
   { "Prompt",                    76,         0,           0,   TRAD_INTEGER,              0 },  // RFC2869 Section 5.10  Prompt
   { "Connect-Info",              77,         0,           0,   TRAD_TEXT,                 0 },  // RFC2869 Section 5.11  Connect-Info
   { "Configuration-Token",       78,         0,           0,   TRAD_TEXT,                 0 },  // RFC2869 Section 5.12  Configuration-Token
   { "EAP-Message",               79,         0,           0,   TRAD_STRING,     TRAD_CONCAT },  // RFC2869 Section 5.13  EAP-Message
   { "Message-Authenticator",     80,         0,           0,   TRAD_STRING,               0 },  // RFC2869 Section 5.14  Message-Authenticator
   { "Tunnel-Private-Group-Id",   81,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.6   Tunnel-Private-Group-Id
   { "Tunnel-Assignment-Id",      82,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.7   Tunnel-Assignment-Id
   { "Tunnel-Preference",         83,         0,           0,   TRAD_INTEGER,   TRAD_HAS_TAG },  // RFC2868 Section 3.8   Tunnel-Preference
   { "Acct-Tunnel-Packets-Lost",  86,         0,           0,   TRAD_INTEGER,              0 },  // RFC2867 Section 4.2   Acct-Tunnel-Packets-Lost
   { "NAS-Port-Id",               87,         0,           0,   TRAD_TEXT,                 0 },  // RFC2869 Section 5.17  NAS-Port-Id
   { "Framed-Pool",               88,         0,           0,   TRAD_TEXT,                 0 },  // RFC2869 Section 5.18  Framed-Pool
   { "Chargeable-User-Identity",  89,         0,           0,   TRAD_STRING,               0 },  // RFC4372 Section 2.2   CUI Attribute
   { "Tunnel-Client-Auth-Id",     90,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.9   Tunnel-Client-Auth-Id
   { "Tunnel-Server-Auth-Id",     91,         0,           0,   TRAD_TEXT,      TRAD_HAS_TAG },  // RFC2868 Section 3.10  Tunnel-Server-Auth-Id
   { "NAS-Filter-Rule",           92,         0,           0,   TRAD_TEXT,                 0 },  // RFC4849 Section 2     NAS-Filter-Rule Attribute
   // unassigned                  93
   { "Originating-Line-Info",     94,         0,           0,   TRAD_STRING,               0 },  // RFC7155 Section 4.2.8 Originating-Line-Info AVP
   { "NAS-IPv6-Address",          95,         0,           0,   TRAD_IPV6ADDR,             0 },  // RFC3162 Section 2.1   NAS-IPv6-Address
   { "Framed-Interface-Id",       96,         0,           0,   TRAD_IFID,                 0 },  // RFC3162 Section 2.2   Framed-Interface-Id (listed in RFC as section 3.2)
   { "Framed-IPv6-Prefix",        97,         0,           0,   TRAD_IPV6PREFIX,           0 },  // RFC3162 Section 2.3   Framed-IPv6-Prefix
   { "Login-IPv6-Host",           98,         0,           0,   TRAD_IPV6ADDR,             0 },  // RFC3162 Section 2.4   Login-IPv6-Host
   { "Framed-IPv6-Route",         99,         0,           0,   TRAD_TEXT,                 0 },  // RFC3162 Section 2.5   Framed-IPv6-Route
   { "Framed-IPv6-Pool",         100,         0,           0,   TRAD_TEXT,                 0 },  // RFC3162 Section 2.6   Framed-IPv6-Pool
   // unassigned                 191
   // Experimental Use           192-223                                                            RFC3575
   // Implementation Specific    224-240                                                            RFC3575
   { "Extended-Attribute-1",     241,         0,           0,   TRAD_EXTENDED,             0 },  // RFC6929 Section 3.1   Extended-Type-1
   { "Extended-Attribute-2",     242,         0,           0,   TRAD_EXTENDED,             0 },  // RFC6929 Section 3.2   Extended-Type-2
   { "Extended-Attribute-3",     243,         0,           0,   TRAD_EXTENDED,             0 },  // RFC6929 Section 3.3   Extended-Type-3
   { "Extended-Attribute-4",     244,         0,           0,   TRAD_EXTENDED,             0 },  // RFC6929 Section 3.4   Extended-Type-4
   { "Extended-Attribute-5",     245,         0,           0,   TRAD_LONG_EXTENDED,        0 },  // RFC6929 Section 3.5   Long-Extended-Type-1
   { "Extended-Attribute-6",     246,         0,           0,   TRAD_LONG_EXTENDED,        0 },  // RFC6929 Section 3.6   Long-Extended-Type-2
   // Reserved                   247-255
   { NULL, 0, 0, 0, 0, 0 }
};


#pragma mark tinyrad_dict_default_values[]
const TinyRadDictValueDef tinyrad_dict_default_values[] =
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

   { "ARAP-Zone-Access",          "Default-Zone",              1 },  // RFC2869 Section 5.6 ARAP-Zone-Access
   { "ARAP-Zone-Access",          "Zone-Filter-Inclusive",     2 },  // RFC2869 Section 5.6 ARAP-Zone-Access
   { "ARAP-Zone-Access",          "Zone-Filter-Exclusive",     4 },  // RFC2869 Section 5.6 ARAP-Zone-Access

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

   { "Ingress-Filters",           "Enabled",                   1 },  // RFC4675 Section 2.2 Ingress-Filters
   { "Ingress-Filters",           "Disabled",                  2 },  // RFC4675 Section 2.2 Ingress-Filters

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

   { "Prompt",                    "No-Echo",                   0 },  // RFC2869 Section 5.10  Prompt
   { "Prompt",                    "Echo",                      1 },  // RFC2869 Section 5.10  Prompt

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

   { "Tunnel-Medium-Type",        "IP",                        1 },  // alias included in FreeRADIUS 3.0.25 (dictionary.rfc2868)
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

/* end of source */
