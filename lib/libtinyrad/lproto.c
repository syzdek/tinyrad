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
#define _LIB_LIBTINYRAD_LPROTO_C 1
#include "lproto.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <assert.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

TinyRadPcktBuff *
tinyrad_pckt_buff_alloc( void );


void
tinyrad_pckt_buff_free(
         TinyRadPcktBuff *             buff );


TinyRadPcktBuff *
tinyrad_pckt_buff_realloc(
         TinyRadPcktBuff *             buff );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

TinyRadPcktBuff *
tinyrad_pckt_buff_alloc( void )
{
   TinyRadPcktBuff * buff;

   if ((buff = malloc(sizeof(TinyRadPcktBuff))) == NULL)
      return(NULL);
   memset(buff, 0, sizeof(TinyRadPcktBuff));

   if ((buff->buf_pckt = malloc(TRAD_PACKET_MAX_LEN)) == NULL)
   {
      free(buff);
      return(NULL);
   };
   memset(buff->buf_pckt, 0, TRAD_PACKET_MAX_LEN);

   buff->buf_size = TRAD_PACKET_MAX_LEN;

   return(buff);
}


void
tinyrad_pckt_buff_free(
         TinyRadPcktBuff *             buff )
{
   if (!(buff))
      return;
   if ((buff->buf_pckt))
      free(buff->buf_pckt);
   free(buff);
   return;
}


TinyRadPcktBuff *
tinyrad_pckt_buff_realloc(
         TinyRadPcktBuff *             buff )
{
   size_t      size;
   void *      ptr;

   // determine size of packet/message
   size = (size_t) ntohs(buff->buf_pckt->pckt_length);
   if (size <= buff->buf_size)
      return(buff);

   // packet size exceeds RFC limits
   if (size > TRAD_TCP_MAX_LEN)
      return(NULL);

   // increase size of buffer to hold packet
   if ((ptr = realloc(buff->buf_pckt, size)) == NULL)
      return(NULL);
   buff->buf_pckt = ptr;
   buff->buf_size = size;

   return(buff);
}


/* end of source */
