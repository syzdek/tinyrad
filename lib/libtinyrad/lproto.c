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

#include "larray.h"
#include "ldict.h"
#include "lmemory.h"
#include "loid.h"


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

int
tinyrad_attr_list_add_vals(
         TinyRadAttrList *             list,
         TinyRadAttrValues **          avp,
         const TinyRadOID *            attr_oid );


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

int
tinyrad_attr_vals_add_binval(
         TinyRadAttrValues *           av,
         const TinyRadBinValue *       binval );


TinyRadAttrValues *
tinyrad_attr_vals_alloc(
         const char *                  name,
         const TinyRadOID *            oid,
         uint8_t                       data_type,
         uint32_t                      flags );


int
tinyrad_attr_vals_cmp_key(
         const TinyRadAttrValues **    obj,
         const TinyRadObj *            oid );


int
tinyrad_attr_vals_cmp_obj(
         const TinyRadAttrValues **    a,
         const TinyRadAttrValues **    b );


void
tinyrad_attr_vals_free(
         TinyRadAttrValues *           av );


ssize_t
tinyrad_attr_vals_index(
         TinyRadAttrList *             list,
         const TinyRadOID *            attr_oid );


TinyRadAttrValues *
tinyrad_attr_vals_lookup(
         TinyRadAttrList *             list,
         const char *                  attr_name,
         const TinyRadOID *            attr_oid );


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

int
tinyrad_attr_list_add(
         TinyRadAttrList *             list,
         const char *                  attr_name,
         TinyRadBinValue *             attr_value )
{
   TinyRadDictAttr *    attr;
   TinyRadAttrValues *  attrvals;
   int                  rc;

   assert(list       != NULL);
   assert(attr_name  != NULL);
   assert(attr_value != NULL);

   // lookup attribute
   if ((attr = tinyrad_dict_attr_lookup(list->dict, attr_name, NULL)) == NULL)
      return(TRAD_EATTRIBUTE);

   // lookup existing or create new attribute value
   if ((attrvals = tinyrad_attr_vals_lookup(list, NULL, attr->oid)) == NULL)
      if ((rc = tinyrad_attr_list_add_vals(list, &attrvals, attr->oid)) != TRAD_SUCCESS)
         return(rc);

   // add binval to attrvals
   if ((rc = tinyrad_attr_vals_add_binval(attrvals, attr_value)) != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
}


int
tinyrad_attr_list_add_vals(
         TinyRadAttrList *             list,
         TinyRadAttrValues **          avp,
         const TinyRadOID *            attr_oid )
{
   void *               ptr;
   ssize_t              rc;
   size_t               width;
   uint8_t              attr_data_type;
   uint32_t             attr_flags;
   unsigned             opts;
   void **              listp;
   size_t *             lenp;
   const char *         attr_name;
   TinyRadDictAttr *    attr;
   TinyRadAttrValues *  av;
   int                  (*compar)(const void *, const void *);

   assert(list       != NULL);
   assert(attr_oid   != NULL);

   // increase size of array
   if ((ptr = realloc(list->attrvals, (sizeof(TinyRadAttrValues *) * (list->attrvals_len+2)))) == NULL)
      return(TRAD_ENOMEM);
   list->attrvals[list->attrvals_len+0] = NULL;
   list->attrvals[list->attrvals_len+1] = NULL;

   // look up attribute name
   attr_name      = NULL;
   attr_flags     = 0;
   attr_data_type = 0;
   if ((attr = tinyrad_dict_attr_lookup(list->dict, NULL, attr_oid)) != NULL)
   {
      attr_name      = attr->name;
      attr_flags     = attr->flags;
      attr_data_type = attr->data_type;
   };

   // allocate new attribute value
   av = tinyrad_attr_vals_alloc(attr_name, attr_oid, attr_data_type, attr_flags);
   if (av == NULL)
      return(TRAD_ENOMEM);

   // copy attribute information
   if ( ((attr)) && ((attr->vendor)) )
   {
      av->type_octs = attr->vendor->type_octs;
      av->len_octs  = attr->vendor->len_octs;
   };

   // parameters for inserting data into array
   listp       = (void **)&list->attrvals;
   lenp        = &list->attrvals_len;
   width       = sizeof(TinyRadAttrValues *);
   opts        = TINYRAD_ARRAY_INSERT;
   compar      = (int(*)(const void*, const void*)) &tinyrad_attr_vals_cmp_obj;

   // save value to list
   if ((rc = tinyrad_array_add(listp, lenp, width, &av, opts, compar, NULL, NULL)) < 0)
   {
      tinyrad_obj_release(&av->obj);
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   };
   tinyrad_obj_retain(&av->obj);

   if ((avp))
      *avp = av;

   return(TRAD_SUCCESS);
}


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

int
tinyrad_attr_vals_add_binval(
         TinyRadAttrValues *           av,
         const TinyRadBinValue *       binval )
{
   size_t         size;
   size_t         len;
   void *         ptr;

   assert(av     != NULL);
   assert(binval != NULL);

   // increase size of array
   len  = tinyrad_binval_list_count(av->values);
   size = sizeof(TinyRadBinValue *) * (len+2);
   if ((ptr = realloc(av->values, size)) == NULL)
      return(TRAD_ENOMEM);
   av->values        = ptr;
   av->values[len+1] = NULL;

   // duplicate value
   if ((av->values[len] = tinyrad_binval_dup(binval)) == NULL)
      return(TRAD_ENOMEM);

   return(TRAD_SUCCESS);
}


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

   if ((av->oid = tinyrad_oid_dup(oid)) == NULL)
   {
      tinyrad_attr_vals_free(av);
      return(NULL);
   };

   av->name = ((name)) ? tinyrad_strdup(name) : tinyrad_oid2str(oid, TRAD_OID_TYPE_ATTRIBUTE);
   if (av->name == NULL)
   {
      tinyrad_attr_vals_free(av);
      return(NULL);
   };

   return(av);
}


int
tinyrad_attr_vals_cmp_key(
         const TinyRadAttrValues **    obj,
         const TinyRadObj *            oid )
{
   int   rc;
   if ((rc = tinyrad_oid_cmp(&(*obj)->oid, &oid)) != 0)
      return(rc);
   return(0);
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


ssize_t
tinyrad_attr_vals_index(
         TinyRadAttrList *             list,
         const TinyRadOID *            attr_oid )
{
   void *               list_ptr;
   size_t               list_len;
   size_t               list_width;
   unsigned             opts;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(list     != NULL);
   assert(attr_oid != NULL);

   list_ptr    = list->attrvals;
   list_len    = list->attrvals_len;
   list_width  = sizeof(TinyRadAttrValues *);
   opts        = TINYRAD_ARRAY_LASTDUP;
   compar      = (int(*)(const void *, const void *))&tinyrad_attr_vals_cmp_key;

   return(tinyrad_array_search(list_ptr, list_len, list_width, attr_oid, opts, NULL, compar));
}


TinyRadAttrValues *
tinyrad_attr_vals_lookup(
         TinyRadAttrList *             list,
         const char *                  attr_name,
         const TinyRadOID *            attr_oid )
{
   TinyRadDictAttr *    attr;
   TinyRadOID *         oid;
   ssize_t              idx;

   TinyRadDebugTrace();

   assert(list != NULL);
   assert( ((attr_name))  || ((attr_oid))  );
   assert( (!(attr_name)) || (!(attr_oid)) );

   // set OID
   oid = NULL;
   if (!(attr_oid))
   {
      if ((attr = tinyrad_dict_attr_lookup(list->dict, attr_name, NULL)) != NULL)
         attr_oid = attr->oid;
      if (!(attr))
      {
         if ((oid = tinyrad_str2oid(attr_name)) == NULL)
            return(NULL);
         attr_oid = oid;
      };
   };

   // search for attribute value
   idx = tinyrad_attr_vals_index(list, attr_oid);
   tinyrad_free(oid);

   return( (idx < 0) ? NULL : list->attrvals[idx] );
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
