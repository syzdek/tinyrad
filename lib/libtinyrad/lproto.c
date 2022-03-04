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

//---------------------------//
// attribute list prototypes //
//---------------------------//
#pragma mark attribute list prototypes

TinyRadAttrList *
tinyrad_attr_list_alloc(
         TinyRadDict *                 dict );


void
tinyrad_attr_list_free(
         TinyRadAttrList *             list );


//----------------------------//
// attribute value prototypes //
//----------------------------//
#pragma mark attribute value prototypes

TinyRadAttrValues *
tinyrad_attr_vals_alloc(
         const char *                  name,
         const TinyRadOID *            oid,
         uint8_t                       data_type,
         uint32_t                      flags );


int
tinyrad_attr_vals_cmp_obj(
         const TinyRadAttrValues **    a,
         const TinyRadAttrValues **    b );


void
tinyrad_attr_vals_free(
         TinyRadAttrValues *           av );


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

//--------------------------//
// attribute list functions //
//--------------------------//
#pragma mark attribute list functions

TinyRadAttrList *
tinyrad_attr_list_alloc(
         TinyRadDict *                 dict )
{
   TinyRadAttrList * list;
   if ((list = tinyrad_obj_alloc(sizeof(TinyRadAttrList), (void(*)(void*))&tinyrad_attr_list_free)) == NULL)
      return(NULL);
   list->dict = ((dict)) ? tinyrad_obj_retain(&dict->obj) : NULL;
   return(list);
}


void
tinyrad_attr_list_free(
         TinyRadAttrList *             list )
{
   size_t pos;

   if (!(list))
      return;

   if ((list->dict))
      tinyrad_obj_release(&list->dict->obj);

   if ((list->attrvals))
   {
      for(pos = 0; (pos < list->attrvals_len); pos++)
         tinyrad_free(list->attrvals[pos]);
      free(list->attrvals);
   };

   free(list);

   return;
}


int
tinyrad_attr_list_initialize(
         TinyRad *                     tr,
         TinyRadAttrList **            listp )
{
   TinyRadAttrList *    list;

   assert(tr    != NULL);
   assert(listp != NULL);

   if ((list = tinyrad_attr_list_alloc(tr->dict)) == NULL)
      return(TRAD_ENOMEM);

   list->dict = tinyrad_obj_retain(&tr->dict->obj);

   *listp = tinyrad_obj_retain(&list->obj);

   return(TRAD_SUCCESS);
}


//---------------------------//
// attribute value functions //
//---------------------------//
#pragma mark attribute value functions

TinyRadAttrValues *
tinyrad_attr_vals_alloc(
         const char *                  name,
         const TinyRadOID *            oid,
         uint8_t                       data_type,
         uint32_t                      flags )
{
   TinyRadAttrValues *     av;

   assert(oid != NULL);

   if ((av = tinyrad_obj_alloc(sizeof(TinyRadAttrValues), (void(*)(void*))&tinyrad_attr_vals_free)) == NULL)
      return(NULL);
   av->type_octs = 1;
   av->len_octs  = 1;
   av->flags     = flags;
   av->data_type = ((data_type)) ? data_type : TRAD_DATATYPE_STRING;

   av->name = ((name)) ? tinyrad_strdup(name) : tinyrad_oid2str(oid, TRAD_OID_TYPE_ATTRIBUTE);
   if (av->name != NULL)
   {
      tinyrad_attr_vals_free(av);
      return(NULL);
   };

   return(NULL);
}


int
tinyrad_attr_vals_cmp_obj(
         const TinyRadAttrValues **    a,
         const TinyRadAttrValues **    b )
{
   int   rc;
   if ((rc = tinyrad_oid_cmp(&(*a)->oid, &(*b)->oid)) != 0)
      return(rc);
   return(0);
}


void
tinyrad_attr_vals_free(
         TinyRadAttrValues *           av )
{
   if (!(av))
      return;

   if ((av->name))
      free(av->name);

   if ((av->oid))
      free(av->oid);

   if ((av->values))
      tinyrad_binval_list_free(av->values);

   free(av);

   return;
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
