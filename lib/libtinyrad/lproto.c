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

#include "ldict.h"
#include "lmemory.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//----------------//
// AVP prototypes //
//----------------//
#pragma mark AVP prototypes

void
tinyrad_avp_free(
         TinyRadAVP *                  avp );


int
tinyrad_avp_initialize(
         TinyRadAVP **                 avpp,
         TinyRadDictAttr *             attr,
         uint8_t                       attr_type,
         uint8_t                       data_type );


//---------------------//
// AVP list prototypes //
//---------------------//
#pragma mark AVP list prototypes

void
tinyrad_avplist_free(
         TinyRadAVPList *              avplist );


//------------------------//
// pckt memory prototypes //
//------------------------//
#pragma mark pckt memory prototypes

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

//---------------//
// AVP functions //
//---------------//
#pragma mark AVP functions

void
tinyrad_avp_free(
         TinyRadAVP *                  avp )
{
   size_t      pos;

   if (!(avp))
      return;

   if ((avp->attr))
      tinyrad_obj_release(&avp->attr->obj);

   if ((avp->values))
      for(pos = 0; (pos < avp->values_len); pos++)
         tinyrad_free(&avp->values[pos]);

   memset(avp, 0, sizeof(TinyRadAVP));
   free(avp);

   return;
}


int
tinyrad_avp_initialize(
         TinyRadAVP **                 avpp,
         TinyRadDictAttr *             attr,
         uint8_t                       attr_type,
         uint8_t                       data_type )
{
   TinyRadAVP *            avp;

   TinyRadDebugTrace();

   assert(avpp != NULL);

   if ((avp = tinyrad_obj_alloc(sizeof(TinyRadAVP), (void(*)(void*))&tinyrad_avp_free)) == NULL)
      return(TRAD_ENOMEM);

   avp->attr_type = attr_type;
   avp->data_type = data_type;

   if ((attr))
   {
      avp->attr      = tinyrad_obj_retain(&attr->obj);
      avp->attr_type = attr->type;
      avp->data_type = attr->data_type;
   };

   *avpp = tinyrad_obj_retain(&avp->obj);

   return(TRAD_SUCCESS);
}


//--------------------//
// AVP list functions //
//--------------------//
#pragma mark AVP list functions

void
tinyrad_avplist_free(
         TinyRadAVPList *              avplist )
{
   size_t pos;
   if ((avplist->list))
      for(pos = 0; (pos < avplist->list_len); pos++)
         tinyrad_obj_release(&avplist->list[pos]->obj);
   memset(avplist, 0, sizeof(TinyRadAVPList));
   free(avplist);
   return;
}


int
tinyrad_avplist_initialize(
         TinyRadAVPList **             avplistp )
{
   TinyRadAVPList * avplist;
   TinyRadDebugTrace();
   assert(avplistp != NULL);
   if ((avplist = tinyrad_obj_alloc(sizeof(TinyRadAVPList), (void(*)(void*))&tinyrad_avplist_free)) == NULL)
      return(TRAD_ENOMEM);
   *avplistp = tinyrad_obj_retain(&avplist->obj);
   return(TRAD_SUCCESS);
}


//-----------------------//
// pckt memory functions //
//-----------------------//
#pragma mark pckt memory functions

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


//----------------------//
// conversion functions //
//----------------------//
#pragma mark conversion functions

uint64_t
tinyrad_htonll(
         uint64_t                      hostlonglong )
{
   return(
      ((uint64_t)((((uint8_t*)&hostlonglong)[0])) << 56) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[1])) << 48) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[2])) << 40) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[3])) << 32) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[4])) << 24) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[5])) << 16) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[6])) <<  8) |
      ((uint64_t)((((uint8_t*)&hostlonglong)[7])) <<  0)
   );
}


uint64_t
tinyrad_ntohll(
         uint64_t                      netlonglong )
{
   return(
      ((uint64_t)((((uint8_t*)&netlonglong)[0])) << 56) |
      ((uint64_t)((((uint8_t*)&netlonglong)[1])) << 48) |
      ((uint64_t)((((uint8_t*)&netlonglong)[2])) << 40) |
      ((uint64_t)((((uint8_t*)&netlonglong)[3])) << 32) |
      ((uint64_t)((((uint8_t*)&netlonglong)[4])) << 24) |
      ((uint64_t)((((uint8_t*)&netlonglong)[5])) << 16) |
      ((uint64_t)((((uint8_t*)&netlonglong)[6])) <<  8) |
      ((uint64_t)((((uint8_t*)&netlonglong)[7])) <<  0)
   );
}


/* end of source */
