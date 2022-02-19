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
#ifndef _LIB_LIBTINYRAD_LPROTO_H
#define _LIB_LIBTINYRAD_LPROTO_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "libtinyrad.h"

#include <stdint.h>
#include <sys/socket.h>


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

typedef struct _tinyrad_pckt_buffer       TinyRadPcktBuff;


typedef struct _tinyrad_avp
{
   TinyRadObj           obj;
   size_t               values_len;
   TinyRadDictAttr *    attr;
   TinyRadBinValue *    values;
} TinyRadAVP;


typedef struct tinyrad_packet
{
   uint8_t              pckt_code;               // RFC 2865 Section 3. Packet Format: Code
   uint8_t              pckt_identifier;         // RFC 2865 Section 3. Packet Format: Identifier
   uint16_t             pckt_length;             // RFC 2865 Section 3. Packet Format: Length
   uint32_t             pckt_authenticator[4];   // RFC 2865 Section 3. Packet Format: Authenticator (specifies "sixteen (16) octets")
   uint8_t              pckt_attrs[];
} tinyrad_packet_t;


typedef struct tinyrad_attr
{
   uint8_t              attr_type;              // RFC 2865 Section 5. Attributes: Type
   uint8_t              attr_len;               // RFC 2865 Section 5. Attributes: Length
   uint8_t              attr_value[];           // RFC 2865 Section 5. Attributes: Value
} tinyrad_attr_t;


typedef struct tinyrad_vsa                      // RFC 2865 5.26. Vendor-Specific
{
   uint8_t              vsa_type;               // RFC 2865 5.26. Vendor-Specific: Type (must be 26)
   uint8_t              vsa_len;                // RFC 2865 5.26. Vendor-Specific: Length (>= 7)
   uint8_t              vsa_vendor_id[4];       // RFC 2865 5.26. Vendor-Specific: Vendor-Id
   uint8_t              vsa_string[];           // RFC 2865 5.26. Vendor-Specific: String
} tinyrad_vsa_t;


struct _tinyrad_pckt_buffer
{
   size_t                  buf_size;
   size_t                  buf_len;
   tinyrad_packet_t *      buf_pckt;
   struct sockaddr_storage buff_sa;
};


typedef struct tinyrad_pckt_vsa_value
{
   uint8_t              val_vendor_id[4];       // RFC 2865 5.26. Vendor-Specific: Vendor-Id
   uint8_t              val_string[];           // RFC 2865 5.26. Vendor-Specific: String
} tinyrad_vsa_value_t;


typedef struct tinyrad_pckt_vsa_value_should
{
   uint8_t              val_vendor_id[4];       // RFC 2865 5.26. Vendor-Specific: Vendor-Id
   uint8_t              val_vendor_type;        // RFC 2865 5.26. Vendor-Specific: String
   uint8_t              val_vendor_len;         // RFC 2865 5.26. Vendor-Specific: String
   uint8_t              val_value[];            // RFC 2865 5.26. Vendor-Specific: String};
} tinyrad_vsa_should_t;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//-----------------------//
// conversion prototypes //
//-----------------------//
#pragma mark conversion prototypes



#endif /* end of header */
