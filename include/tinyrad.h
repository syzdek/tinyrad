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


// copies uint16_t as network byte order to buffer (host to packet short)
#define tinyrad_htops( dst, val ) \
         {  ((uint8_t*)(dst))[0] = (uint8_t)(((uint16_t)(val)) >>  8); \
            ((uint8_t*)(dst))[1] = (uint8_t)(((uint16_t)(val)) >>  0); \
         }


// copies uint32_t as network byte order to buffer (host to packet long)
#define tinyrad_htopl( dst, val ) \
         {  ((uint8_t*)(dst))[0] = (uint8_t)(((uint32_t)(val)) >> 24); \
            ((uint8_t*)(dst))[1] = (uint8_t)(((uint32_t)(val)) >> 16); \
            ((uint8_t*)(dst))[2] = (uint8_t)(((uint32_t)(val)) >>  8); \
            ((uint8_t*)(dst))[3] = (uint8_t)(((uint32_t)(val)) >>  0); \
         }


// copies uint64_t as network byte order to buffer (host to packet long long)
#define tinyrad_htopll( dst, val ) \
         {  ((uint8_t*)(dst))[0] = (uint8_t)(((uint64_t)(val)) >> 56); \
            ((uint8_t*)(dst))[1] = (uint8_t)(((uint64_t)(val)) >> 48); \
            ((uint8_t*)(dst))[2] = (uint8_t)(((uint64_t)(val)) >> 40); \
            ((uint8_t*)(dst))[3] = (uint8_t)(((uint64_t)(val)) >> 32); \
            ((uint8_t*)(dst))[4] = (uint8_t)(((uint64_t)(val)) >> 24); \
            ((uint8_t*)(dst))[5] = (uint8_t)(((uint64_t)(val)) >> 16); \
            ((uint8_t*)(dst))[6] = (uint8_t)(((uint64_t)(val)) >>  8); \
            ((uint8_t*)(dst))[7] = (uint8_t)(((uint64_t)(val)) >>  0); \
         }


// reads uint16_t from buffer as host byte order (packet to host short)
#define tinyrad_ptohs( src ) \
         (  (uint64_t) \
            (  ((uint16_t)((((uint8_t*)(src))[0])) << 8) | \
               ((uint16_t)((((uint8_t*)(src))[1])) << 0) \
            ) \
         )


// reads uint32_t from buffer as host byte order (packet to host long)
#define tinyrad_ptohl( src ) \
         (  (uint32_t) \
            (  ((uint32_t)((((uint8_t*)(src))[0])) << 24) | \
               ((uint32_t)((((uint8_t*)(src))[1])) << 16) | \
               ((uint32_t)((((uint8_t*)(src))[2])) <<  8) | \
               ((uint32_t)((((uint8_t*)(src))[3])) <<  0) \
            ) \
         )


// reads uint32_t from buffer as host byte order (packet to host long long)
#define tinyrad_ptohll( src ) \
         (  (uint64_t) \
            (  ((uint64_t)((((uint8_t*)(src))[0])) << 56) | \
               ((uint64_t)((((uint8_t*)(src))[1])) << 48) | \
               ((uint64_t)((((uint8_t*)(src))[2])) << 40) | \
               ((uint64_t)((((uint8_t*)(src))[3])) << 32) | \
               ((uint64_t)((((uint8_t*)(src))[4])) << 24) | \
               ((uint64_t)((((uint8_t*)(src))[5])) << 16) | \
               ((uint64_t)((((uint8_t*)(src))[6])) <<  8) | \
               ((uint64_t)((((uint8_t*)(src))[7])) <<  0) \
            ) \
         )


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

// library error codes
#define TRAD_SUCCESS                0x0000 ///< success
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
#define TRAD_ESCHEME                0x000c ///< invalid or mismatched URL scheme
#define TRAD_EINVAL                 0x000d ///< invalid argument
#define TRAD_EASSUMPTION            0x000e ///< invalid developer assumption

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

#define TRAD_NO                     0
#define TRAD_YES                    1

// library get/set options
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

// dictionary get options
#define TRAD_DICT_OPT_REF_COUNT           1  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_NAME                2  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_TYPE                3  // used by TinyRadDictAttr and TinyRadDictValue
#define TRAD_DICT_OPT_VEND_ID             4  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_VEND_TYPE           5  // used by TinyRadDictVendor, TinyRadDictAttr, and TinyRadDictValue
#define TRAD_DICT_OPT_TYPE_OCTS           6  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_LEN_OCTS            7  // used by TinyRadDictVendor and TinyRadDictAttr
#define TRAD_DICT_OPT_DATA_TYPE           8  // used by TinyRadDictAttr
#define TRAD_DICT_OPT_FLAGS               9  // used by TinyRadDictAttr
#define TRAD_DICT_OPT_DATA               10  // used by TinyRadDictValue
#define TRAD_DICT_OPT_ATTR_NAME          11  // used by TinyRadDictAttr and TinyRadDictValue
#define TRAD_DICT_OPT_OID                12  // used by TinyRadDictAttr and TinyRadDictValue

#define TRAD_OID_OPT_ATTRIBUTE            1
#define TRAD_OID_OPT_VALUE                2
#define TRAD_OID_OPT_VENDOR               3

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

#define TRAD_PACKET_MAX_LEN         4096           // RFC 2865 Section 3. Packet Format: Length
#define TRAD_PACKET_MIN_LEN         20             // RFC 2865 Section 3. Packet Format: Length
#define TRAD_TCP_MAX_LEN            65535          // RFC 7930 Section 2. Changes to Packet Processing
#define TRAD_TCP_MIN_LEN            TRAD_PACKET_MIN_LEN

#define TRAD_SECRET_RADSEC_TCP      "radsec"       // RFC 6614 Section 2.3: Connection Setup
#define TRAD_SECRET_RADSEC_UDP      "radius/dtls"  // RFC 7360 Section 2.1: Changes to RADIUS

// RADIUS Attributes
#define  TRAD_ATTR_USER_NAME                     1  // RFC2865 Section 5.1     User-Name
#define  TRAD_ATTR_USER_PASSWORD                 2  // RFC2865 Section 5.2     User-Password
#define  TRAD_ATTR_CHAP_PASSWORD                 3  // RFC2865 Section 5.3     CHAP-Password
#define  TRAD_ATTR_NAS_IP_ADDRESS                4  // RFC2865 Section 5.4     NAS-IP-Address
#define  TRAD_ATTR_NAS_PORT                      5  // RFC2865 Section 5.5     NAS-Port
#define  TRAD_ATTR_SERVICE_TYPE                  6  // RFC2865 Section 5.6     Service-Type
#define  TRAD_ATTR_FRAMED_PROTOCOL               7  // RFC2865 Section 5.7     Framed-Protocol
#define  TRAD_ATTR_FRAMED_IP_ADDRESS             8  // RFC2865 Section 5.8     Framed-IP-Address
#define  TRAD_ATTR_FRAMED_IP_NETMASK             9  // RFC2865 Section 5.9     Framed-IP-Netmask
#define  TRAD_ATTR_FRAMED_ROUTING               10  // RFC2865 Section 5.10    Framed-Routing
#define  TRAD_ATTR_FILTER_ID                    11  // RFC2865 Section 5.11    Filter-Id
#define  TRAD_ATTR_FRAMED_MTU                   12  // RFC2865 Section 5.12    Framed-MTU
#define  TRAD_ATTR_FRAMED_COMPRESSION           13  // RFC2865 Section 5.13    Framed-Compression
#define  TRAD_ATTR_LOGIN_IP_HOST                14  // RFC2865 Section 5.14    Login-IP-Host
#define  TRAD_ATTR_LOGIN_SERVICE                15  // RFC2865 Section 5.15    Login-Service
#define  TRAD_ATTR_LOGIN_TCP_PORT               16  // RFC2865 Section 5.16    Login-TCP-Port
#define  TRAD_ATTR_REPLY_MESSAGE                18  // RFC2865 Section 5.18    Reply-Message
#define  TRAD_ATTR_CALLBACK_NUMBER              19  // RFC2865 Section 5.19    Callback-Number
#define  TRAD_ATTR_CALLBACK_ID                  20  // RFC2865 Section 5.20    Callback-Id
#define  TRAD_ATTR_FRAMED_ROUTE                 22  // RFC2865 Section 5.22    Framed-Route
#define  TRAD_ATTR_FRAMED_IPX_NETWORK           23  // RFC2865 Section 5.23    Framed-IPX-Network
#define  TRAD_ATTR_STATE                        24  // RFC2865 Section 5.24    State
#define  TRAD_ATTR_CLASS                        25  // RFC2865 Section 5.25    Class
#define  TRAD_ATTR_VENDOR_SPECIFIC              26  // RFC2865 Section 5.26    Vendor-Specific
#define  TRAD_ATTR_SESSION_TIMEOUT              27  // RFC2865 Section 5.27    Session-Timeout
#define  TRAD_ATTR_IDLE_TIMEOUT                 28  // RFC2865 Section 5.28    Idle-Timeout
#define  TRAD_ATTR_TERMINATION_ACTION           29  // RFC2865 Section 5.29    Termination-Action
#define  TRAD_ATTR_CALLED_STATION_ID            30  // RFC2865 Section 5.30    Called-Station-Id
#define  TRAD_ATTR_CALLING_STATION_ID           31  // RFC2865 Section 5.31    Calling-Station-Id
#define  TRAD_ATTR_NAS_IDENTIFIER               32  // RFC2865 Section 5.32    NAS-Identifier
#define  TRAD_ATTR_PROXY_STATE                  33  // RFC2865 Section 5.33    Proxy-State
#define  TRAD_ATTR_LOGIN_LAT_SERVICE            34  // RFC2865 Section 5.34    Login-LAT-Service
#define  TRAD_ATTR_LOGIN_LAT_NODE               35  // RFC2865 Section 5.35    Login-LAT-Node
#define  TRAD_ATTR_LOGIN_LAT_GROUP              36  // RFC2865 Section 5.36    Login-LAT-Group
#define  TRAD_ATTR_FRAMED_APPLETALK_LINK        37  // RFC2865 Section 5.37    Framed-AppleTalk-Link
#define  TRAD_ATTR_FRAMED_APPLETALK_NETWORK     38  // RFC2865 Section 5.38    Framed-AppleTalk-Network
#define  TRAD_ATTR_FRAMED_APPLETALK_ZONE        39  // RFC2865 Section 5.39    Framed-AppleTalk-Zone
#define  TRAD_ATTR_ACCT_STATUS_TYPE             40  // RFC2866 Section 5.1     Acct-Status-Type
#define  TRAD_ATTR_ACCT_DELAY_TIME              41  // RFC2866 Section 5.2     Acct-Delay-Time
#define  TRAD_ATTR_ACCT_INPUT_OCTETS            42  // RFC2866 Section 5.3     Acct-Input-Octets
#define  TRAD_ATTR_ACCT_OUTPUT_OCTETS           43  // RFC2866 Section 5.4     Acct-Output-Octets
#define  TRAD_ATTR_ACCT_SESSION_ID              44  // RFC2866 Section 5.5     Acct-Session-Id
#define  TRAD_ATTR_ACCT_AUTHENTIC               45  // RFC2866 Section 5.6     Acct-Authentic
#define  TRAD_ATTR_ACCT_SESSION_TIME            46  // RFC2866 Section 5.7     Acct-Session-Time
#define  TRAD_ATTR_ACCT_INPUT_PACKETS           47  // RFC2866 Section 5.8     Acct-Input-Packets
#define  TRAD_ATTR_ACCT_OUTPUT_PACKETS          48  // RFC2866 Section 5.9     Acct-Output-Packets
#define  TRAD_ATTR_ACCT_TERMINATE_CAUSE         49  // RFC2866 Section 5.10    Acct-Terminate-Cause
#define  TRAD_ATTR_ACCT_MULTI_SESSION_ID        50  // RFC2866 Section 5.11    Acct-Multi-Session-Id
#define  TRAD_ATTR_ACCT_LINK_COUNT              51  // RFC2866 Section 5.12    Acct-Link-Count
#define  TRAD_ATTR_ACCT_INPUT_GIGAWORDS         52  // RFC2869 Section 5.1     Acct-Input-Gigawords
#define  TRAD_ATTR_ACCT_OUTPUT_GIGAWORDS        53  // RFC2869 Section 5.2     Acct-Output-Gigawords
#define  TRAD_ATTR_EVENT_TIMESTAMP              55  // RFC2869 Section 5.3     Event-Timestamp
#define  TRAD_ATTR_EGRESS_VLANID                56  // RFC4675 Section 2.1     Egress-VLANID
#define  TRAD_ATTR_INGRESS_FILTERS              57  // RFC4675 Section 2.2     Ingress-Filters
#define  TRAD_ATTR_EGRESS_VLAN_NAME             58  // RFC4675 Section 2.3     Egress-VLAN-Name
#define  TRAD_ATTR_USER_PRIORITY_TABLE          59  // RFC4675 Section 2.4     User-Priority-Table
#define  TRAD_ATTR_CHAP_CHALLENGE               60  // RFC2865 Section 5.40    CHAP-Challenge
#define  TRAD_ATTR_NAS_PORT_TYPE                61  // RFC2865 Section 5.41    NAS-Port-Type
#define  TRAD_ATTR_PORT_LIMIT                   62  // RFC2865 Section 5.42    Port-Limit
#define  TRAD_ATTR_LOGIN_LAT_PORT               63  // RFC2865 Section 5.43    Login-LAT-Port
#define  TRAD_ATTR_TUNNEL_TYPE                  64  // RFC2868 Section 3.1     Tunnel-Type
#define  TRAD_ATTR_TUNNEL_MEDIUM_TYPE           65  // RFC2868 Section 3.2     Tunnel-Medium-Type
#define  TRAD_ATTR_TUNNEL_CLIENT_ENDPOINT       66  // RFC2868 Section 3.3     Tunnel-Client-Endpoint
#define  TRAD_ATTR_TUNNEL_SERVER_ENDPOINT       67  // RFC2868 Section 3.4     Tunnel-Server-Endpoint
#define  TRAD_ATTR_ACCT_TUNNEL_CONNECTION       68  // RFC2867 Section 4.1     Acct-Tunnel-Connection
#define  TRAD_ATTR_TUNNEL_PASSWORD              69  // RFC2868 Section 3.5     Tunnel-Password
#define  TRAD_ATTR_ARAP_PASSWORD                70  // RFC2869 Section 5.4     ARAP-Password
#define  TRAD_ATTR_ARAP_FEATURES                71  // RFC2869 Section 5.5     ARAP-Features
#define  TRAD_ATTR_ARAP_ZONE_ACCESS             72  // RFC2869 Section 5.6     ARAP-Zone-Access
#define  TRAD_ATTR_ARAP_SECURITY                73  // RFC2869 Section 5.7     ARAP-Security
#define  TRAD_ATTR_ARAP_SECURITY_DATA           74  // RFC2869 Section 5.8     ARAP-Security-Data
#define  TRAD_ATTR_PASSWORD_RETRY               75  // RFC2869 Section 5.9     Password-Retry
#define  TRAD_ATTR_PROMPT                       76  // RFC2869 Section 5.10    Prompt
#define  TRAD_ATTR_CONNECT_INFO                 77  // RFC2869 Section 5.11    Connect-Info
#define  TRAD_ATTR_CONFIGURATION_TOKEN          78  // RFC2869 Section 5.12    Configuration-Token
#define  TRAD_ATTR_EAP_MESSAGE                  79  // RFC2869 Section 5.13    EAP-Message
#define  TRAD_ATTR_MESSAGE_AUTHENTICATOR        80  // RFC2869 Section 5.14    Message-Authenticator
#define  TRAD_ATTR_TUNNEL_PRIVATE_GROUP_ID      81  // RFC2868 Section 3.6     Tunnel-Private-Group-Id
#define  TRAD_ATTR_TUNNEL_ASSIGNMENT_ID         82  // RFC2868 Section 3.7     Tunnel-Assignment-Id
#define  TRAD_ATTR_TUNNEL_PREFERENCE            83  // RFC2868 Section 3.8     Tunnel-Preference
#define  TRAD_ATTR_ACCT_TUNNEL_PACKETS_LOST     86  // RFC2867 Section 4.2     Acct-Tunnel-Packets-Lost
#define  TRAD_ATTR_NAS_PORT_ID                  87  // RFC2869 Section 5.17    NAS-Port-Id
#define  TRAD_ATTR_FRAMED_POOL                  88  // RFC2869 Section 5.18    Framed-Pool
#define  TRAD_ATTR_CHARGEABLE_USER_IDENTITY     89  // RFC4372 Section 2.2     CUI Attribute
#define  TRAD_ATTR_TUNNEL_CLIENT_AUTH_ID        90  // RFC2868 Section 3.9     Tunnel-Client-Auth-Id
#define  TRAD_ATTR_TUNNEL_SERVER_AUTH_ID        91  // RFC2868 Section 3.10    Tunnel-Server-Auth-Id
#define  TRAD_ATTR_NAS_FILTER_RULE              92  // RFC4849 Section 2       NAS-Filter-Rule Attribute
#define  TRAD_ATTR_ORIGINATING_LINE_INFO        94  // RFC7155 Section 4.2.8   Originating-Line-Info AVP
#define  TRAD_ATTR_NAS_IPV6_ADDRESS             95  // RFC3162 Section 2.1     NAS-IPv6-Address
#define  TRAD_ATTR_FRAMED_INTERFACE_ID          96  // RFC3162 Section 2.2     Framed-Interface-Id (listed in RFC as section 3.2)
#define  TRAD_ATTR_FRAMED_IPV6_PREFIX           97  // RFC3162 Section 2.3     Framed-IPv6-Prefix
#define  TRAD_ATTR_LOGIN_IPV6_HOST              98  // RFC3162 Section 2.4     Login-IPv6-Host
#define  TRAD_ATTR_FRAMED_IPV6_ROUTE            99  // RFC3162 Section 2.5     Framed-IPv6-Route
#define  TRAD_ATTR_FRAMED_IPV6_POOL            100  // RFC3162 Section 2.6     Framed-IPv6-Pool
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_1        241  // RFC6929 Section 3.1     Extended-Type-1
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_2        242  // RFC6929 Section 3.2     Extended-Type-2
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_3        243  // RFC6929 Section 3.3     Extended-Type-3
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_4        244  // RFC6929 Section 3.4     Extended-Type-4
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_5        245  // RFC6929 Section 3.5     Long-Extended-Type-1
#define  TRAD_ATTR_EXTENDED_ATTRIBUTE_6        246  // RFC6929 Section 3.6     Long-Extended-Type-2

// RADIUS Attribute Data Type Definitions
#define TRAD_DATATYPE_INTEGER        1 ///< RFC 8044 Section 3.1.  integer:       32-bit unsigned integer
#define TRAD_DATATYPE_ENUM           2 ///< RFC 8044 Section 3.2.  enum:          32-bit unsigned integer enumerated types
#define TRAD_DATATYPE_TIME           3 ///< RFC 8044 Section 3.3.  time:          32-bit unsigned integer in seconds since 00:00:00 UTC, January 1, 1970
#define TRAD_DATATYPE_TEXT           4 ///< RFC 8044 Section 3.4.  text:          encoded UTF-8 text
#define TRAD_DATATYPE_STRING         5 ///< RFC 8044 Section 3.5.  string:        binary data
#define TRAD_DATATYPE_CONCAT         6 ///< RFC 8044 Section 3.6.  concat:        more than 253 octets of binary data
#define TRAD_DATATYPE_IFID           7 ///< RFC 8044 Section 3.7.  ifid:          8-octet IPv6 Interface Identifier (hex:hex:hex:hex)
#define TRAD_DATATYPE_IPV4ADDR       8 ///< RFC 8044 Section 3.8.  ipv4addr:      IPv4 address in network byte order
#define TRAD_DATATYPE_IPV6ADDR       9 ///< RFC 8044 Section 3.9.  ipv6addr:      IPv6 address in network byte order
#define TRAD_DATATYPE_IPV6PREFIX    10 ///< RFC 8044 Section 3.10. ipv6prefix:    encoded an IPv6 prefix
#define TRAD_DATATYPE_IPV4PREFIX    11 ///< RFC 8044 Section 3.11. ipv4prefix:    encoded an IPv4 prefix
#define TRAD_DATATYPE_INTEGER64     12 ///< RFC 8044 Section 3.12. integer64:     64-bit unsigned integer
#define TRAD_DATATYPE_TLV           13 ///< RFC 8044 Section 3.13. tlv:           Type-Length-Value, allows nested attributes (RFC 6929: Section 2.3 TLV Data Type)
#define TRAD_DATATYPE_VSA           14 ///< RFC 8044 Section 3.14. vsa:           vendor-specific data, see [RFC2865], Section 5.26
#define TRAD_DATATYPE_EXTENDED      15 ///< RFC 8044 Section 3.15. extended:      "Extended Type" format, see [RFC6929], Section 2.1
#define TRAD_DATATYPE_LONGEXTENDED  16 ///< RFC 8044 Section 3.16. long-extended: "Long Extended Type" format, see [RFC6929], Section 2.2
#define TRAD_DATATYPE_ETHER        128 ///< FreeRADIUS 3.x Dictionary: ether:      Ethernet MAC address
#define TRAD_DATATYPE_ABINARY      129 ///< FreeRADIUS 3.x Dictionary: abinary:    Ascend binary filter format
#define TRAD_DATATYPE_BYTE         130 ///< FreeRADIUS 3.x Dictionary: byte:       8-bit unsigned integer
#define TRAD_DATATYPE_SHORT        131 ///< FreeRADIUS 3.x Dictionary: short       16-bit unsigned integer
#define TRAD_DATATYPE_SIGNED       132 ///< FreeRADIUS 3.x Dictionary: signed      31-bit signed integer (packed into 32-bit field)


// RADIUS Attribute flags
#define TRAD_FLG_ENCRYPT1           0x0001 ///< encrypted with the method as defined in RFC2865 for the User-Password attribute
#define TRAD_FLG_ENCRYPT2           0x0002 ///< encrypted with the method as defined in RFC2868 for the Tunnel-Password attribute
#define TRAD_FLG_ENCRYPT3           0x0004 ///< encrypted as per Ascend's definitions for the Ascend-Send-Secret attribute
#define TRAD_FLG_ENCRYPT_MASK       0x00FF ///< encrypt method mask
#define TRAD_FLG_HAS_TAG            0x0100 ///< attribute may have tag as defined in RFC 2868
#define TRAD_FLG_CONCAT             0x0200 ///< concatenate values


// RADIUS codes
#define TRAD_ACCESS_REQ             1     // RFC 2865 Section 4.1.  Access-Request
#define TRAD_ACCESS_ACCEPT          2     // RFC 2865 Section 4.2.  Access-Accept
#define TRAD_ACCESS_REJECT          3     // RFC 2865 Section 4.3.  Access-Reject
#define TRAD_ACCOUNT_REQ            4     // RFC 2866 Section 4.1.  Accounting-Request
#define TRAD_ACCOUNT_RES            5     // RFC 2866 Section 4.2.  Accounting-Response
#define TRAD_ACCESS_CHALLENGE      11     // RFC 2865 Section 4.4.  Access-Challenge
#define TRAD_DISCONNECT_REQ        40     // RFC 3576 Section 2.3.  Packet Format
#define TRAD_DISCONNECT_ACK        41     // RFC 3576 Section 2.3.  Packet Format
#define TRAD_DISCONNECT_NAK        42     // RFC 3576 Section 2.3.  Packet Format
#define TRAD_COA_REQ               43     // RFC 3576 Section 2.3.  Packet Format
#define TRAD_COA_ACK               44     // RFC 3576 Section 2.3.  Packet Format
#define TRAD_COA_NAK               45     // RFC 3576 Section 2.3.  Packet Format


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _tinyrad                   TinyRad;
typedef struct _tinyrad_dict              TinyRadDict;
typedef struct _tinyrad_dict_attr         TinyRadDictAttr;
typedef struct _tinyrad_dict_value        TinyRadDictValue;
typedef struct _tinyrad_dict_vendor       TinyRadDictVendor;
typedef struct _tinyrad_avp_list          TinyRadAVPList;
typedef struct _tinyrad_oid               TinyRadOID;
typedef struct sockaddr_storage           tinyrad_sockaddr_t;


typedef struct tinyrad_binval
{
    size_t              bv_len;
    uint8_t             bv_val[];
} TinyRadBinValue;


typedef struct tinyrad_dict_attr_def
{
   const char *          name;
   uintptr_t             type;
   uint32_t              vendor_id;
   uint32_t              vendor_type;
   uint32_t              data_type;
   uint32_t              flags;
} TinyRadDictAttrDef;


typedef struct tinyrad_dict_value_def
{
   const char *          attr_name;
   const char *          value_name;
   uint64_t              data;
} TinyRadDictValueDef;


typedef struct tinyrad_dict_vendor_def
{
   const char *          name;
   uintptr_t             vendor_id;
   uint64_t              vendor_type_octs;
   uint64_t              vendor_len_octs;
} TinyRadDictVendorDef;


typedef struct tinyrad_map
{
   const char *          map_name;
   uintptr_t             map_value;
} TinyRadMap;


// Support RADIUS URLs
//    radius://hostport/secret[?proto]          (default proto: udp, port: 1812) [RFC2865]
//    radius-acct://hostport/secret[?proto]     (default proto: udp, port: 1813) [RFC2866]
//    radsec://hostport/[?proto]                (default proto: tcp, port: 2083) [RFC6614/RFC7360]
//    radius-dynauth://hostport/secret[?proto]  (default proto: udp, port: 3799) [RFC5176]
typedef struct tinyrad_url_desc
{
   char *                        trud_host;
   char *                        trud_secret;
   int                           trud_port;
   unsigned                      trud_opts;
   /* may contain additional fields for internal use */
   struct tinyrad_url_desc *     trud_next;
   tinyrad_sockaddr_t **         trud_sockaddrs;
   size_t                        trud_sockaddrs_len;
} TinyRadURLDesc;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//------------------//
// debug prototypes //
//------------------//
#pragma mark debug prototypes

_TINYRAD_F const char *
tinyrad_lib_name( void );


_TINYRAD_F const char *
tinyrad_lib_version(
         unsigned *                    currentp,
         unsigned *                    revisionp,
         unsigned *                    agep );


_TINYRAD_F const char *
tinyrad_pkg_name( void );


_TINYRAD_F const char *
tinyrad_pkg_version(
         unsigned *                    majorp,
         unsigned *                    minorp,
         unsigned *                    patchp,
         const char **                 buildp );


//-----------------------//
// dictionary prototypes //
//-----------------------//
#pragma mark dictionary prototypes

_TINYRAD_F int
tinyrad_dict_add_path(
         TinyRadDict *                 dict,
         const char *                  path );


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


_TINYRAD_F int
tinyrad_dict_import(
         TinyRadDict *                 dict,
         const TinyRadDictVendorDef *  vendor_defs,
         const TinyRadDictAttrDef *    attr_defs,
         const TinyRadDictValueDef *   value_defs,
         char ***                      msgsp,
         uint32_t                      opts );


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


_TINYRAD_F TinyRadDictValue *
tinyrad_dict_value_get(
         TinyRadDict *                 dict,
         const char *                  name,
         TinyRadDictAttr *             attr,
         uint8_t                       type,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type,
         uint64_t                      data );


_TINYRAD_F int
tinyrad_dict_value_info(
         TinyRadDictValue *            value,
         int                           param,
         void *                        outvalue );


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
// error prototypes //
//------------------//
#pragma mark error prototypes

_TINYRAD_F const char *
tinyrad_strerror(
         int                           errnum );


_TINYRAD_F int
tinyrad_strerror_r(
         int                           errnum,
         char *                        strerrbuf,
         size_t                        buflen );


//-------------------//
// memory prototypes //
//-------------------//
#pragma mark memory prototypes

_TINYRAD_F TinyRadBinValue *
tinyrad_binval_alloc(
         size_t                        size );


_TINYRAD_F TinyRadBinValue *
tinyrad_binval_dup(
         const TinyRadBinValue *       ptr );


_TINYRAD_F TinyRadBinValue *
tinyrad_binval_realloc(
         TinyRadBinValue *             ptr,
         size_t                        size );


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


//----------------//
// OID prototypes //
//----------------//
#pragma mark OID prototypes

_TINYRAD_F char *
tinyrad_oid2str(
         const TinyRadOID *            oid,
         unsigned                      opts );


_TINYRAD_F int
tinyrad_oid_cmp(
         const void *                 a,
         const void *                 b );


_TINYRAD_F TinyRadOID *
tinyrad_oid_init(
         uint32_t *                    vals,
         size_t                        len );


_TINYRAD_F int
tinyrad_oid_values(
         const TinyRadOID *            oid,
         uint32_t **                   valsp,
         size_t *                      lenp );


//---------------------//
// protocol prototypes //
//---------------------//
#pragma mark protocol prototypes

_TINYRAD_F int
tinyrad_avplist_initialize(
         TinyRadAVPList **             avplistp );


uint64_t
tinyrad_htonll(
         uint64_t                      hostlonglong );


uint64_t
tinyrad_ntohll(
         uint64_t                      netlonglong );


//-------------------//
// string prototypes //
//-------------------//
#pragma mark string prototypes

_TINYRAD_F void
tinyrad_strsfree(
         char **                       strs );


//----------------//
// URL prototypes //
//----------------//
#pragma mark URL prototypes

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
