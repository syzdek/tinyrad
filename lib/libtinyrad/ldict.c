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
#include <assert.h>

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

#define TRAD_DICT_KEYWORD_INCLUDE           1
#define TRAD_DICT_KEYWORD_ATTRIBUTE         2
#define TRAD_DICT_KEYWORD_VALUE             3
#define TRAD_DICT_KEYWORD_VENDOR            4
#define TRAD_DICT_KEYWORD_BEGIN_VENDOR      5
#define TRAD_DICT_KEYWORD_END_VENDOR        6


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _tinyrad_dict_key TinyRadDictKey;

struct _tinyrad_dict_key
{
   const char *         str;
   void *               padptr;
   uint32_t             type;
   uint32_t             vendor_id;
   uint32_t             vendor_type;
   uint32_t             pad32;
   uint64_t             value_data;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//----------------------------//
// dictionary misc prototypes //
//----------------------------//
#pragma mark dictionary misc prototypes

int
tinyrad_dict_add_attr(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr );


int
tinyrad_dict_add_value(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr,
         TinyRadDictValue *            value );


int
tinyrad_dict_add_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor );


void
tinyrad_dict_free(
         TinyRadDict *                dict );


//---------------------------------//
// dictionary attribute prototypes //
//---------------------------------//
#pragma mark dictionary attribute prototypes

static uint8_t
__attr_len_octs(
         const TinyRadDictAttr *       attr );


static uint8_t
__attr_type_octs(
         const TinyRadDictAttr *       attr );


static uint32_t
__attr_vendor_id(
         const TinyRadDictAttr *       attr );


TinyRadDictAttr *
tinyrad_dict_attr_alloc(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_type,
         uint8_t                       data_type,
         uint32_t                      flags );


int
tinyrad_dict_attr_cmp_key_name(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_attr_cmp_key_type(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_attr_cmp_key_vendor(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_attr_cmp_obj_name(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_attr_cmp_obj_type(
         const void *                 a,
         const void *                 b );


void
tinyrad_dict_attr_free(
         TinyRadDictAttr *             attr );


ssize_t
tinyrad_dict_attr_index(
         TinyRadDict *                dict,
         const char *                 name,
         uint8_t                      type,
         uint32_t                     vendor_id,
         uint32_t                     vendor_type,
         int                          by_vendor );


//------------------------------//
// dictionary import prototypes //
//------------------------------//
#pragma mark dictionary import prototypes


//-----------------------------//
// dictionary parse prototypes //
//-----------------------------//
#pragma mark dictionary parse prototypes

int
tinyrad_dict_parse_attribute(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor,
         TinyRadFile *                file,
         uint32_t                     opts );


int
tinyrad_dict_parse_begin_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file );


int
tinyrad_dict_parse_end_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file );


int
tinyrad_dict_parse_include(
         TinyRadDict *                dict,
         TinyRadFile **               filep );


int
tinyrad_dict_parse_value(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts );


int
tinyrad_dict_parse_vendor(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts );


//-----------------------------//
// dictionary print prototypes //
//-----------------------------//
#pragma mark dictionary print prototypes

void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         size_t                        attr_idx );


void
tinyrad_dict_print_value(
         TinyRadDict *                 dict,
         size_t                        value_idx );


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         size_t                        vendor_idx );


//-----------------------------//
// dictionary value prototypes //
//-----------------------------//
#pragma mark dictionary value prototypes

uint8_t
__value_type(
         const TinyRadDictValue *      value );


uint32_t
__value_vendor_id(
         const TinyRadDictValue *      value );


uint32_t
__value_vendor_type(
         const TinyRadDictValue *      value );


TinyRadDictValue *
tinyrad_dict_value_alloc(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr,
         const char *                  name,
         uint64_t                      data );


int
tinyrad_dict_value_cmp_key_attr(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_value_cmp_key_data(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_value_cmp_key_name(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_value_cmp_obj_attr(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_value_cmp_obj_data(
         const void *                  a,
         const void *                  b );


int
tinyrad_dict_value_cmp_obj_name(
         const void *                  a,
         const void *                  b );


void
tinyrad_dict_value_free(
         TinyRadDictValue *           value );


ssize_t
tinyrad_dict_value_index(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type,
         uint64_t                      value_data,
         int                           by_attr );


//------------------------------//
// dictionary vendor prototypes //
//------------------------------//
#pragma mark dictionary vendor prototypes

TinyRadDictVendor *
tinyrad_dict_vendor_alloc(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     id,
         uint8_t                      type_octs,
         uint8_t                      len_octs );


int
tinyrad_dict_vendor_cmp_key_id(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_vendor_cmp_key_name(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_vendor_cmp_obj_id(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_vendor_cmp_obj_name(
         const void *                  a,
         const void *                  b );


void
tinyrad_dict_vendor_free(
         TinyRadDictVendor *          vendor );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_dict_data_type[]
static const TinyRadMap tinyrad_dict_data_type[] =
{
//   FreeRADIUS Name   RFC Name
   { "ABINARY",        TRAD_DATATYPE_ABINARY},
   { "BYTE",           TRAD_DATATYPE_BYTE},
   { "CONCAT",         TRAD_DATATYPE_CONCAT},
   { "DATE",           TRAD_DATATYPE_TIME},
   { "ETHER",          TRAD_DATATYPE_ETHER},
   { "EXTENDED",       TRAD_DATATYPE_EXTENDED},
   { "IFID",           TRAD_DATATYPE_IFID},
   { "INTEGER",        TRAD_DATATYPE_INTEGER},
   { "INTEGER64",      TRAD_DATATYPE_INTEGER64},
   { "IPADDR",         TRAD_DATATYPE_IPV4ADDR},
   { "IPV4PREFIX",     TRAD_DATATYPE_IPV4PREFIX},
   { "IPV6ADDR",       TRAD_DATATYPE_IPV6ADDR},
   { "IPV6PREFIX",     TRAD_DATATYPE_IPV6PREFIX},
   { "LONGEXTENDED",   TRAD_DATATYPE_LONGEXTENDED},
   { "OCTETS",         TRAD_DATATYPE_STRING},            // a FreeRADIUS dictionary refers to a RADDIUS string as "octets"
   { "RAD_TLV",        TRAD_DATATYPE_TLV},
   { "SHORT",          TRAD_DATATYPE_SHORT},
   { "SIGNED",         TRAD_DATATYPE_SIGNED},
   { "STRING",         TRAD_DATATYPE_TEXT },             // a FreeRADIUS dictionary refers to a RADDIUS text as "string"
   { "VSA",            TRAD_DATATYPE_VSA },
   { NULL, 0 }
};


#pragma mark tinyrad_dict_attr_flags[]
static const TinyRadMap tinyrad_dict_attr_flags[] =
{
   { "encrypt=1",    TRAD_ENCRYPT1},
   { "encrypt=2",    TRAD_ENCRYPT2},
   { "encrypt=3",    TRAD_ENCRYPT3},
   { "has_tag",      TRAD_HAS_TAG},
   { "concat",       TRAD_CONCAT},
   { NULL, 0 }
};


#pragma mark tinyrad_dict_options[]
static const TinyRadMap tinyrad_dict_options[] =
{
   { "$INCLUDE",        TRAD_DICT_KEYWORD_INCLUDE},
   { "ATTRIBUTE",       TRAD_DICT_KEYWORD_ATTRIBUTE},
   { "BEGIN-VENDOR",    TRAD_DICT_KEYWORD_BEGIN_VENDOR},
   { "END-VENDOR",      TRAD_DICT_KEYWORD_END_VENDOR},
   { "VALUE",           TRAD_DICT_KEYWORD_VALUE},
   { "VENDOR",          TRAD_DICT_KEYWORD_VENDOR},
   { NULL, 0 }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

//---------------------------//
// dictionary misc functions //
//---------------------------//
#pragma mark dictionary misc functions

int
tinyrad_dict_add_attr(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr )
{
   size_t               size;
   size_t               width;
   void *               ptr;
   TinyRadDictAttr *    old;
   ssize_t              rc;
   unsigned             opts;

   TinyRadDebugTrace();

   assert(dict      != NULL);
   assert(attr      != NULL);

   // verify attribute doesn't exist
   if ((old = tinyrad_dict_attr_lookup(dict, attr->name, 0, 0, 0)) != NULL)
   {
      if ( (attr->flags|TRAD_DFLT_ATTR) != (old->flags|TRAD_DFLT_ATTR) )
         return(TRAD_EEXISTS);
      if (attr->type != old->type)
         return(TRAD_EEXISTS);
      if (__attr_vendor_id(attr) != __attr_vendor_id(old))
         return(TRAD_EEXISTS);
      if (attr->vendor_type != old->vendor_type)
         return(TRAD_EEXISTS);
      if (attr->data_type != old->data_type)
         return(TRAD_EEXISTS);
      return(TRAD_SUCCESS);
   };
   if ((old = tinyrad_dict_attr_lookup(dict, NULL, attr->type, __attr_vendor_id(attr), attr->vendor_type)) != NULL)
   {
      if ( (attr->flags|TRAD_DFLT_ATTR) != (old->flags|TRAD_DFLT_ATTR) )
         return(TRAD_EEXISTS);
      if (attr->type != old->type)
         return(TRAD_EEXISTS);
      if (__attr_vendor_id(attr) != __attr_vendor_id(old))
         return(TRAD_EEXISTS);
      if (attr->vendor_type != old->vendor_type)
         return(TRAD_EEXISTS);
      if (attr->data_type != old->data_type)
         return(TRAD_EEXISTS);
   };

   // resize attribute name list in dictionary
   size = sizeof(TinyRadDictAttr *) * (dict->attrs_name_len+1);
   if ((ptr = realloc(dict->attrs_name, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->attrs_name = ptr;

   // resize attribute type list in dictionary
   size = sizeof(TinyRadDictAttr *) * (dict->attrs_type_len+1);
   if ((ptr = realloc(dict->attrs_type, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->attrs_type = ptr;

   width = sizeof(TinyRadDictAttr *);
   opts  = TINYRAD_ARRAY_INSERT | TINYRAD_ARRAY_LASTDUP;

   // save attribute by name to dictionary
   if ((rc = tinyrad_array_add((void **)&dict->attrs_name, &dict->attrs_name_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_name, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&attr->obj);

   // save attribute by type to dictionary
   if ((rc = tinyrad_array_add((void **)&dict->attrs_type, &dict->attrs_type_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_type, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&attr->obj);

   return(TRAD_SUCCESS);
}


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

   TinyRadDebugTrace();

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


int
tinyrad_dict_add_value(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr,
         TinyRadDictValue *            value )
{
   size_t               size;
   size_t               width;
   ssize_t              rc;
   void *               ptr;
   unsigned             opts;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict  != NULL);
   assert(attr  != NULL);
   assert(value != NULL);

   // increase size of value by name list in dictionary
   size = sizeof(TinyRadDictValue *) * (dict->values_name_len+1);
   if ((ptr = realloc(dict->values_name, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->values_name = ptr;

    // increase size of value by data list in dictionary
   size = sizeof(TinyRadDictValue *) * (dict->values_data_len+1);
   if ((ptr = realloc(dict->values_data, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->values_data = ptr;

   // save value by name in dictionary
   opts     = TINYRAD_ARRAY_INSERT;
   width    = sizeof(TinyRadDictValue *);
   compar   = &tinyrad_dict_value_cmp_obj_name;
   if ((rc = tinyrad_array_add((void **)&dict->values_name, &dict->values_name_len, width, &value, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&value->obj);

   // save value by data in dictionary
   opts     = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_LASTDUP;
   width    = sizeof(TinyRadDictValue *);
   compar   = &tinyrad_dict_value_cmp_obj_data;
   if ((rc = tinyrad_array_add((void **)&dict->values_data, &dict->values_data_len, width, &value, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&value->obj);

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_add_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor )
{
   size_t               size;
   size_t               width;
   ssize_t              rc;
   unsigned             opts;
   void *               ptr;
   TinyRadDictVendor *  old;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict   != NULL);
   assert(vendor != NULL);

   // verify attribute doesn't exist
   if ((old = tinyrad_dict_vendor_lookup(dict, vendor->name, 0)) != NULL)
   {
      if (vendor->id != old->id)
         return(TRAD_EEXISTS);
      if (vendor->type_octs != old->type_octs)
         return(TRAD_EEXISTS);
      if (vendor->len_octs != old->len_octs)
         return(TRAD_EEXISTS);
      return(TRAD_SUCCESS);
   };
   if ((old = tinyrad_dict_vendor_lookup(dict, NULL, vendor->id)) != NULL)
   {
      if (vendor->id != old->id)
         return(TRAD_EEXISTS);
      if (vendor->type_octs != old->type_octs)
         return(TRAD_EEXISTS);
      if (vendor->len_octs != old->len_octs)
         return(TRAD_EEXISTS);
   };

   // resize vendor by name list
   size = sizeof(TinyRadDictVendor *) * (dict->vendors_name_len + 2);
   if ((ptr = realloc(dict->vendors_name, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors_name = ptr;

   // resize vendor by id list
   size = sizeof(TinyRadDictVendor *) * (dict->vendors_id_len + 2);
   if ((ptr = realloc(dict->vendors_id, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors_id = ptr;

   // save vendor by name
   opts     = TINYRAD_ARRAY_INSERT;
   width    = sizeof(TinyRadDictVendor *);
   compar   = &tinyrad_dict_vendor_cmp_obj_name;
   if ((rc = tinyrad_array_add((void **)&dict->vendors_name, &dict->vendors_name_len, width, &vendor, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&vendor->obj);

   // save value by id
   opts     = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_LASTDUP;
   width    = sizeof(TinyRadDictVendor *);
   compar   = &tinyrad_dict_vendor_cmp_obj_id;
   if ((rc = tinyrad_array_add((void **)&dict->vendors_id, &dict->vendors_id_len, width, &vendor, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   tinyrad_obj_retain(&vendor->obj);

   return(0);
}


/// Initialize dicitionary file buffer
///
/// @param[in]  dict          dictionary reference
void
tinyrad_dict_free(
         TinyRadDict *                dict )
{
   size_t        pos;

   TinyRadDebugTrace();

   if (!(dict))
      return;

   // free attributes
   if ((dict->attrs_name))
   {
      for(pos = 0; (pos < dict->attrs_name_len); pos++)
         tinyrad_obj_release(&dict->attrs_name[pos]->obj);
      free(dict->attrs_name);
   };
   if ((dict->attrs_type))
   {
      for(pos = 0; (pos < dict->attrs_type_len); pos++)
         tinyrad_obj_release(&dict->attrs_type[pos]->obj);
      free(dict->attrs_type);
   };

   // free paths
   if ((dict->paths))
   {
      for(pos = 0; (pos < dict->paths_len); pos++)
         free(dict->paths[pos]);
      free(dict->paths);
   };

   // free values
   if ((dict->values_data))
   {
      for(pos = 0; (pos < dict->values_data_len); pos++)
         tinyrad_obj_release(&dict->values_data[pos]->obj);
      free(dict->values_data);
   };
   if ((dict->values_name))
   {
      for(pos = 0; (pos < dict->values_name_len); pos++)
         tinyrad_obj_release(&dict->values_name[pos]->obj);
      free(dict->values_name);
   };

   // free vendors
   if ((dict->vendors_name))
   {
      for(pos = 0; (pos < dict->vendors_name_len); pos++)
         tinyrad_obj_release(&dict->vendors_name[pos]->obj);
      free(dict->vendors_name);
   };
   if ((dict->vendors_id))
   {
      for(pos = 0; (pos < dict->vendors_id_len); pos++)
         tinyrad_obj_release(&dict->vendors_id[pos]->obj);
      free(dict->vendors_id);
   };

   memset(dict, 0, sizeof(TinyRadDict));
   free(dict);

   return;
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

   TinyRadDebugTrace();

   assert(dictp != NULL);

   if ((dict = tinyrad_obj_alloc(sizeof(TinyRadDict), (void(*)(void*))&tinyrad_dict_free)) == NULL)
      return(TRAD_ENOMEM);
   dict->opts = opts;

   // initializes paths
   if ((dict->paths = malloc(sizeof(char *))) == NULL)
   {
      tinyrad_dict_free(dict);
      return(TRAD_ENOMEM);
   };
   dict->paths[0] = NULL;

   *dictp = tinyrad_obj_retain(&dict->obj);

   return(TRAD_SUCCESS);
}


//--------------------------------//
// dictionary attribute functions //
//--------------------------------//
#pragma mark dictionary attribute functions

static uint8_t
__attr_len_octs(
         const TinyRadDictAttr *       attr )
{
   assert(attr != NULL);
   return( ((attr->vendor)) ? attr->vendor->len_octs : 0);
}


static uint8_t
__attr_type_octs(
         const TinyRadDictAttr *       attr )
{
   assert(attr != NULL);
   return( ((attr->vendor)) ? attr->vendor->type_octs : 0);
}


static uint32_t
__attr_vendor_id(
         const TinyRadDictAttr *       attr )
{
   assert(attr != NULL);
   return( ((attr->vendor)) ? attr->vendor->id : 0);
}


TinyRadDictAttr *
tinyrad_dict_attr_alloc(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_type,
         uint8_t                       data_type,
         uint32_t                      flags )
{
   TinyRadDictAttr *    attr;

   if ((attr = tinyrad_obj_alloc(sizeof(TinyRadDictAttr), (void(*)(void*))&tinyrad_dict_attr_free)) == NULL)
      return(NULL);

   if ((attr->name = strdup(name)) == NULL)
   {
      tinyrad_dict_attr_free(attr);
      return(NULL);
   };

   attr->vendor = tinyrad_obj_retain(&vendor->obj);

   dict->obj_count++;
   attr->order       = dict->obj_count;
   attr->type        = type;
   attr->data_type   = data_type;
   attr->flags       = flags;
   attr->vendor_type = ((vendor)) ? vendor_type : 0;

   return(tinyrad_obj_retain(&attr->obj));
}


int
tinyrad_dict_attr_cmp_key_name(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictAttr * const *  obj = ptr;
   const char *                     dat = key;
   return(strcasecmp( (*obj)->name, dat));
}


int
tinyrad_dict_attr_cmp_key_type(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictAttr * const *     obj = ptr;
   const TinyRadDictKey *              dat = key;

   if ((*obj)->type != dat->type)
      return( ((*obj)->type < dat->type) ? -1 : 1 );

   if (__attr_vendor_id(*obj) != dat->vendor_id)
      return( (__attr_vendor_id(*obj) < dat->vendor_id) ? -1 : 1 );

   if ((*obj)->vendor_type != dat->vendor_type)
      return( ((*obj)->vendor_type < dat->vendor_type) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_attr_cmp_key_vendor(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictAttr * const *     obj = ptr;
   const TinyRadDictKey *              dat = key;

   if ((*obj)->type != dat->type)
      return( ((*obj)->type < dat->type) ? -1 : 1 );

   if (__attr_vendor_id(*obj) != dat->vendor_id)
      return( (__attr_vendor_id(*obj) < dat->vendor_id) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_attr_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictAttr * const * x = a;
   const TinyRadDictAttr * const * y = b;

   if ( (!((*x)->name)) && (!((*y)->name)) )
      return(0);

   if (!((*x)->name))
      return(-1);

   if (!((*y)->name))
      return(1);

   return(strcasecmp( (*x)->name, (*y)->name));
}


int
tinyrad_dict_attr_cmp_obj_type(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictAttr * const * x = a;
   const TinyRadDictAttr * const * y = b;

   if ((*x)->type != (*y)->type)
      return( ((*x)->type < (*y)->type) ? -1 : 1 );

   if (__attr_vendor_id(*x) != __attr_vendor_id(*y))
      return( (__attr_vendor_id(*x) < __attr_vendor_id(*y)) ? -1 : 1 );

   if ((*x)->vendor_type != (*y)->vendor_type)
      return( ((*x)->vendor_type < (*y)->vendor_type) ? -1 : 1 );

   if ((*x)->order != (*y)->order)
      return( ((*x)->order < (*y)->order) ? -1 : 1 );

   return(0);
}


/// Destroys and frees resources of a RADIUS dictionary attribute
///
/// @param[in]  attr          reference to dictionary attribute
void
tinyrad_dict_attr_free(
         TinyRadDictAttr *             attr )
{
   TinyRadDebugTrace();

   if (!(attr))
      return;

   if ((attr->vendor))
      tinyrad_obj_release(&attr->vendor->obj);

   if ((attr->name))
      free(attr->name);

   memset(attr, 0, sizeof(TinyRadDictAttr));
   free(attr);

   return;
}


TinyRadDictAttr *
tinyrad_dict_attr_get(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type )
{
   TinyRadDictAttr * attr;
   TinyRadDebugTrace();
   assert(dict   != NULL);
   vendor_id = ((vendor)) ? vendor->id : vendor_id;
   attr = tinyrad_dict_attr_lookup(dict, name, type, vendor_id, vendor_type);
   return(tinyrad_obj_retain(&attr->obj));
}


ssize_t
tinyrad_dict_attr_index(
         TinyRadDict *                dict,
         const char *                 name,
         uint8_t                      type,
         uint32_t                     vendor_id,
         uint32_t                     vendor_type,
         int                          by_vendor )
{
   size_t               width;
   size_t               len;
   unsigned             opts;
   const void *         key;
   TinyRadDictAttr **   list;
   TinyRadDictKey       attr_type;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert((by_vendor == TRAD_NO) || (by_vendor == TRAD_YES));

   width = sizeof(TinyRadDictAttr *);

   if ((name))
   {
      key      = name;
      len      = dict->attrs_name_len;
      list     = dict->attrs_name;
      compar   = &tinyrad_dict_attr_cmp_key_name;
      opts     = TINYRAD_ARRAY_LASTDUP;
   } else {
      memset(&attr_type, 0, sizeof(attr_type));
      attr_type.type          = type;
      attr_type.vendor_id     = vendor_id;
      attr_type.vendor_type   = vendor_type;
      key      = &attr_type;
      len      = dict->attrs_type_len;
      list     = dict->attrs_type;
      //                                  Lookup attrobute by type          Lookup attribute by vendor
      opts     = (by_vendor == TRAD_NO) ? TINYRAD_ARRAY_LASTDUP           : TINYRAD_ARRAY_FIRSTDUP;
      compar   = (by_vendor == TRAD_NO) ? &tinyrad_dict_attr_cmp_key_type : &tinyrad_dict_attr_cmp_key_vendor;
   };

   return(tinyrad_array_search(list, len, width, key, opts, NULL, compar));
}


int
tinyrad_dict_attr_info(
         TinyRadDictAttr *             attr,
         int                           param,
         void *                        outvalue )
{
   unsigned   uval;

   TinyRadDebugTrace();

   if ( (!(attr)) || (!(outvalue)) )
      return(TRAD_EINVAL);

   // get attribute options
   switch(param)
   {
      case TRAD_DICT_OPT_DATA_TYPE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_DATA_TYPE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", (uint32_t)attr->data_type);
      *((uint32_t *)outvalue) = (uint32_t)attr->data_type;
      break;

      case TRAD_DICT_OPT_FLAGS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_FLAGS, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", attr->flags);
      *((uint32_t *)outvalue) = attr->flags;
      break;

      case TRAD_DICT_OPT_LEN_OCTS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_LEN_OCTS, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", __attr_len_octs(attr));
      *((uint8_t *)outvalue) = __attr_len_octs(attr);
      break;

      case TRAD_DICT_OPT_ATTR_NAME:
      case TRAD_DICT_OPT_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", attr->name);
      if ( (*((char **)outvalue) = strdup(attr->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_DICT_OPT_REF_COUNT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_REF_COUNT, outvalue )", __func__);
      uval = (unsigned)tinyrad_obj_retain_count(&attr->obj);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %u", uval);
      *((unsigned *)outvalue) = uval;
      break;

      case TRAD_DICT_OPT_TYPE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_TYPE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", attr->type);
      *((uint8_t *)outvalue) = attr->type;
      break;

      case TRAD_DICT_OPT_TYPE_OCTS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_TYPE_OCTS, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", __attr_type_octs(attr));
      *((uint8_t *)outvalue) = __attr_type_octs(attr);
      break;

      case TRAD_DICT_OPT_VEND_ID:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_ID, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", __attr_vendor_id(attr));
      *((uint32_t *)outvalue) = __attr_vendor_id(attr);
      break;

      case TRAD_DICT_OPT_VEND_TYPE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_TYPE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", attr->vendor_type);
      *((uint32_t *)outvalue) = attr->vendor_type;
      break;

      default:
      return(TRAD_EINVAL);
   };

   return(TRAD_SUCCESS);
}


TinyRadDictAttr *
tinyrad_dict_attr_lookup(
         TinyRadDict *                dict,
         const char *                 name,
         uint8_t                      type,
         uint32_t                     vendor_id,
         uint32_t                     vendor_type )
{
   size_t               width;
   size_t               len;
   unsigned             opts;
   const void *         key;
   TinyRadDictAttr **   list;
   TinyRadDictAttr **   res;
   TinyRadDictKey       attr_type;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict   != NULL);

   width = sizeof(TinyRadDictAttr *);
   opts     = TINYRAD_ARRAY_LASTDUP;

   if ((name))
   {
      key      = name;
      len      = dict->attrs_name_len;
      list     = dict->attrs_name;
      compar   = &tinyrad_dict_attr_cmp_key_name;
   } else {
      memset(&attr_type, 0, sizeof(attr_type));
      attr_type.type          = type;
      attr_type.vendor_id     = vendor_id;
      attr_type.vendor_type   = vendor_type;
      key      = &attr_type;
      len      = dict->attrs_type_len;
      list     = dict->attrs_type;
      compar   = &tinyrad_dict_attr_cmp_key_type;
   };

   res = tinyrad_array_get(list, len, width, key, opts, compar);
   return( ((res)) ? *res : NULL);
}


//-----------------------------//
// dictionary import functions //
//-----------------------------//
#pragma mark dictionary import functions

int
tinyrad_dict_defaults(
         TinyRadDict *                 dict,
         char ***                      msgsp,
         uint32_t                      opts )
{
   TinyRadDebugTrace();
   assert(dict != NULL);
   assert(opts == 0);
   return(tinyrad_dict_import(dict, tinyrad_dict_default_vendors, tinyrad_dict_default_attrs, tinyrad_dict_default_values, msgsp, opts));
}


int
tinyrad_dict_import(
         TinyRadDict *                 dict,
         const TinyRadDictVendorDef *  vendor_defs,
         const TinyRadDictAttrDef *    attr_defs,
         const TinyRadDictValueDef *   value_defs,
         char ***                      msgsp,
         uint32_t                      opts )
{
   int                     rc;
   size_t                  pos;
   uint8_t                 type;
   uint8_t                 data_type;
   uint32_t                flags;
   uint64_t                data;
   uint32_t                vendor_id;
   uint32_t                vendor_type;
   uint8_t                 type_octs;
   uint8_t                 len_octs;
   const char *            attr_name;
   const char *            value_name;
   const char *            vendor_name;
   TinyRadDictValue *      value;
   TinyRadDictVendor *     vendor;
   TinyRadDictAttr *       attr;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(opts == 0);

   if ((msgsp))
      *msgsp = NULL;

   if ((vendor_defs))
   {
      for(pos = 0; ((vendor_defs[pos].name)); pos++)
      {
         vendor_name = vendor_defs[pos].name;
         vendor_id   = (uint32_t)vendor_defs[pos].vendor_id;
         type_octs   = (uint8_t)vendor_defs[pos].vendor_type_octs;
         len_octs    = (uint8_t)vendor_defs[pos].vendor_len_octs;
         if ((vendor = tinyrad_dict_vendor_alloc(dict, vendor_name, vendor_id, type_octs, len_octs)) == NULL)
            return(tinyrad_error_msgs(TRAD_ENOMEM, msgsp, "out of virtual memory"));
         rc = tinyrad_dict_add_vendor(dict, vendor);
         tinyrad_obj_release(&vendor->obj);
         if (rc != TRAD_SUCCESS)
            return(tinyrad_error_msgs(rc, msgsp, "default attribute %s(%" PRIu32 "): ", vendor_name, vendor_id));
      };
   };

   if ((attr_defs))
   {
      for(pos = 0; ((attr_defs[pos].name)); pos++)
      {
         attr_name      = attr_defs[pos].name;
         type           = (uint8_t)attr_defs[pos].type;
         vendor_id      = attr_defs[pos].vendor_id;
         vendor_type    = attr_defs[pos].vendor_type;
         data_type      = (uint8_t)attr_defs[pos].data_type;
         flags          = (uint32_t)attr_defs[pos].flags;
         flags         |= TRAD_DFLT_ATTR;
         vendor         = tinyrad_dict_vendor_lookup(dict, NULL, vendor_id);
         assert( ((vendor)) || (!(vendor_id)) );
         if ((attr = tinyrad_dict_attr_alloc(dict, attr_name, type, vendor, vendor_type, data_type, flags)) == NULL)
            return(tinyrad_error_msgs(TRAD_ENOMEM, msgsp, "out of virtual memory"));
         rc = tinyrad_dict_add_attr(dict, attr);
         tinyrad_obj_release(&attr->obj);
         if (rc != TRAD_SUCCESS)
            return(tinyrad_error_msgs(rc, msgsp, "default attribute %s(%" PRIu32 "): ", attr_name, type));
      };
   };

   if ((value_defs))
   {
      attr = NULL;
      for(pos = 0; ((value_defs[pos].attr_name)); pos++)
      {
         attr_name  = value_defs[pos].attr_name;
         value_name = value_defs[pos].value_name;
         data       = value_defs[pos].data;
         if ((attr))
            if ((strcasecmp(attr_name, attr->name)))
               attr = NULL;
         if (!(attr))
            attr = tinyrad_dict_attr_lookup(dict, attr_name, 0, 0, 0);
         if (!(attr))
            return(tinyrad_error_msgs(TRAD_ENOENT, msgsp, "default value: %s %s(%" PRIu64 "): ", attr_name, value_name, data));
         if ((value = tinyrad_dict_value_alloc(dict, attr, value_name, data)) == NULL)
            return(tinyrad_error_msgs(TRAD_ENOMEM, msgsp, "out of virtual memory"));
         rc = tinyrad_dict_add_value(dict,attr, value);
         tinyrad_obj_release(&value->obj);
         if (rc != TRAD_SUCCESS)
            return(tinyrad_error_msgs(rc, msgsp, "default value: %s %s(%" PRIu64 "): ", attr_name, value_name, data));
      };
   };

   return(TRAD_SUCCESS);
}


//----------------------------//
// dictionary parse functions //
//----------------------------//
#pragma mark dictionary parse functions

/// Imports file into dictionary
///
/// @param[in]  dict          dictionary reference
/// @param[in]  path          path to import file
/// @param[out] msgsp         pointer to generated error details
/// @param[in]  opts          import options
/// @return returns error code
int
tinyrad_dict_parse(
         TinyRadDict *                dict,
         const char *                 path,
         char ***                     msgsp,
         uint32_t                     opts )
{
   int                   rc;
   TinyRadFile *         file;
   TinyRadFile *         parent;
   TinyRadDictVendor *   vendor;

   TinyRadDebugTrace();

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
         case TRAD_DICT_KEYWORD_ATTRIBUTE:
         if ((rc = tinyrad_dict_parse_attribute(dict, vendor, file, opts)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_KEYWORD_BEGIN_VENDOR:
         if ((rc = tinyrad_dict_parse_begin_vendor(dict, &vendor, file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_KEYWORD_END_VENDOR:
         if ((rc = tinyrad_dict_parse_end_vendor(dict, &vendor, file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_KEYWORD_INCLUDE:
         if ((rc = tinyrad_dict_parse_include(dict, &file)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_KEYWORD_VALUE:
         if ((rc = tinyrad_dict_parse_value(dict, file, opts)) != TRAD_SUCCESS)
         {
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         break;

         case TRAD_DICT_KEYWORD_VENDOR:
         if ((rc = tinyrad_dict_parse_vendor(dict, file, opts)) != TRAD_SUCCESS)
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
tinyrad_dict_parse_attribute(
         TinyRadDict *                dict,
         TinyRadDictVendor *          vendor,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   TinyRadDictAttr *    attr;
   uint8_t              data_type;
   uint8_t              type;
   uint32_t             flags;
   uint32_t             flag;
   uint32_t             vendor_type;
   int                  rc;
   char *               ptr;
   char *               str;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if ( (file->argc < 4) || (file->argc > 5) )
      return(TRAD_ESYNTAX);

   if ((data_type = (uint8_t)tinyrad_map_lookup_name(tinyrad_dict_data_type, file->argv[3], NULL)) == 0)
      return(TRAD_ESYNTAX);

   if ((vendor_type = (uint32_t)strtoul(file->argv[2], &ptr, 10)) == 0)
      return(TRAD_ESYNTAX);

   if ((ptr[0] != '\0') || (file->argv[2] == ptr))
      return(TRAD_ESYNTAX);

   flags = 0;
   if (file->argc == 5)
   {
      ptr = file->argv[4];
      while ((str = ptr) != NULL)
      {
         if ((ptr = strchr(str, ',')) != NULL)
            ptr[0] = '\0';
         if ((flag = (uint32_t)tinyrad_map_lookup_name(tinyrad_dict_attr_flags, str, NULL)) == 0)
            return(TRAD_ESYNTAX);
         if ( ((flag & TRAD_ENCRYPT_MASK) != 0) && ((flags & TRAD_ENCRYPT_MASK) != 0) )
            return(TRAD_ESYNTAX);
         flags |= flag;
         ptr = ((ptr)) ? &ptr[1] : NULL;
      };
   };

   type        = ((vendor)) ? 26          : (uint8_t)vendor_type;
   vendor_type = ((vendor)) ? vendor_type : 0;

   if ((attr = tinyrad_dict_attr_alloc(dict, file->argv[1], type, vendor, vendor_type, data_type, flags)) == NULL)
      return(TRAD_ENOMEM);
   rc = tinyrad_dict_add_attr(dict, attr);
   tinyrad_obj_release(&attr->obj);
   return(rc);
}


int
tinyrad_dict_parse_begin_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file )
{
   TinyRadDebugTrace();

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
tinyrad_dict_parse_end_vendor(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         TinyRadFile *                file )
{
   TinyRadDebugTrace();

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
tinyrad_dict_parse_include(
         TinyRadDict *                dict,
         TinyRadFile **               filep )
{
   int            rc;
   TinyRadFile *  incl;

   TinyRadDebugTrace();

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
tinyrad_dict_parse_value(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   TinyRadDictAttr *    attr;
   TinyRadDictValue *   value;
   uint64_t             data;
   int                  rc;
   char *               ptr;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if (file->argc == 3)
      return(TRAD_ESYNTAX);
   if ((attr = tinyrad_dict_attr_lookup(dict, file->argv[1], 0, 0, 0)) == NULL)
      return(TRAD_ESYNTAX);
   data = (uint64_t)strtoull(file->argv[3], &ptr, 0);
   if ((ptr[0] != '\0') || (file->argv[3] == ptr))
      return(TRAD_ESYNTAX);

   if ((value = tinyrad_dict_value_alloc(dict, attr, file->argv[2], data)) == NULL)
      return(TRAD_ENOMEM);

   rc = tinyrad_dict_add_value(dict, attr, value);
   tinyrad_obj_release(&value->obj);
   return(rc);
}


/// Imports file into dictionary
///
/// @param[in]  dict          dictionary reference
/// @param[in]  file          path to import file
/// @param[in]  opts          import options
/// @return returns error code
int
tinyrad_dict_parse_vendor(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   int                     rc;
   char *                  endptr;
   char *                  str;
   uint32_t                id;
   uint8_t                 type_octs;
   uint8_t                 len_octs;

   TinyRadDictVendor *     vendor;

   TinyRadDebugTrace();

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
      str = strchr(file->argv[3], '=');
      str++;

      // parse type length
      type_octs = (uint8_t)strtoul(str, &endptr, 10);
      if (endptr == str)
         return(TRAD_ESYNTAX);
      if (endptr[0] != ',')
         return(TRAD_ESYNTAX);
      str = &endptr[1];
      if ( (type_octs != 1) && (type_octs != 2) && (type_octs != 4) )
         return(TRAD_ESYNTAX);

      // parse value length
      len_octs = (uint8_t)strtoul(str, &endptr, 10);
      if (endptr == str)
         return(TRAD_ESYNTAX);
      if (endptr[0] != '\0')
         return(TRAD_ESYNTAX);
      if (len_octs > 2)
         return(TRAD_ESYNTAX);
   };

   // initialize vendor struct
   if ((vendor = tinyrad_dict_vendor_alloc(dict, file->argv[1], id, type_octs, len_octs)) == NULL)
      return(TRAD_ENOMEM);
   rc = tinyrad_dict_add_vendor(dict, vendor);
   tinyrad_obj_release(&vendor->obj);
   return(rc);
}


//----------------------------//
// dictionary print functions //
//----------------------------//
#pragma mark dictionary print functions

void
tinyrad_dict_print(
         TinyRadDict *                 dict,
         uint32_t                      opts )
{
   size_t            pos;
   TinyRadDictKey    key;

   TinyRadDebugTrace();

   assert(dict   != NULL);
   assert(opts   != 0);

   printf("# dictionary processed with %s (%s)\n", PACKAGE_NAME, PACKAGE_VERSION);
   printf("#\n");

   // print attribute definitions
   memset(&key, 0, sizeof(key));
   for(pos = 0; (pos < dict->attrs_type_len); pos ++)
   {
      key.type          = dict->attrs_type[pos]->type;
      key.vendor_id     = __attr_vendor_id(dict->attrs_type[pos]);
      key.vendor_type   = dict->attrs_type[pos]->vendor_type;
      if ((key.vendor_id))
         continue;
      if ( ((pos + 1) < dict->attrs_type_len) && (!(tinyrad_dict_attr_cmp_key_type(&dict->attrs_type[pos+1], &key))) )
         continue;
      tinyrad_dict_print_attribute(dict, pos);
   };

   // print vendor definitions
   for(pos = 0; (pos < dict->vendors_id_len); pos++)
   {
      if ( ((pos + 1) < dict->vendors_id_len) && (dict->vendors_id[pos]->id == dict->vendors_id[pos+1]->id) )
         continue;
      tinyrad_dict_print_vendor(dict, pos);
   };

   printf("\n");
   printf("# end of processed dictionary\n");

   return;
}


void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         size_t                        attr_idx )
{
   size_t               pos;
   size_t               flags;
   char                 flagstr[128];
   char                 datatype[32];
   const char *         str;
   ssize_t              value_idx;
   TinyRadDictKey       key;
   TinyRadDictAttr *    attr;

   TinyRadDebugTrace();

   assert(dict     != NULL);
   assert(attr_idx  < dict->attrs_type_len);

   attr = dict->attrs_type[attr_idx];

   flags      = 0;
   flagstr[0] = '\0';
   for(pos = 0; ((tinyrad_dict_attr_flags[pos].name)); pos++)
   {
      if ((attr->flags & tinyrad_dict_attr_flags[pos].value) != 0)
      {
         strncat(flagstr, (((flags)) ? "," : " "), (sizeof(flagstr)-strlen(flagstr)-1));
         strncat(flagstr, tinyrad_dict_attr_flags[pos].name, (sizeof(flagstr)-strlen(flagstr)-1));
         flags++;
      };
   };

   str = tinyrad_map_lookup_value(tinyrad_dict_data_type, attr->data_type, NULL);
   str = ((str)) ? str : "unknown";
   for(pos = 0; ((str[pos])); pos++)
      datatype[pos] = ((str[pos] >= 'A')&&(str[pos] <= 'Z')) ? (str[pos] - 'A' + 'a') : str[pos];
   datatype[pos] = '\0';

   printf("ATTRIBUTE     %-31s %-21" PRIu32 " %s%s\n", attr->name, (((__attr_vendor_id(attr))) ? attr->vendor_type : attr->type), datatype, flagstr);

   memset(&key, 0, sizeof(key));
   key.type          = attr->type;
   key.vendor_id     = __attr_vendor_id(attr);
   key.vendor_type   = ((attr->vendor)) ? attr->vendor_type : 0;

   if ((value_idx = tinyrad_dict_value_index(dict, NULL, key.type, key.vendor_id, key.vendor_type, 0, TRAD_YES)) < 0)
      return;

   tinyrad_dict_print_value(dict, value_idx);

   return;
}


void
tinyrad_dict_print_value(
         TinyRadDict *                 dict,
         size_t                        value_idx )
{
   size_t               pos;
   ssize_t              attr_idx;
   TinyRadDictKey       key;
   TinyRadDictAttr *    attr;
   TinyRadDictValue *   value;

   TinyRadDebugTrace();

   assert(dict     != NULL);
   assert(value_idx  < dict->values_data_len);

   value = dict->values_data[value_idx];
   memset(&key, 0, sizeof(key));
   key.type          = __value_type(value);
   key.vendor_id     = __value_vendor_id(value);
   key.vendor_type   = __value_vendor_type(value);

   for(pos = value_idx; ((pos > 0) && (!(tinyrad_dict_value_cmp_key_attr(&dict->values_data[pos-1], &key)))); pos--);
   value_idx = pos;

   if ((attr_idx = tinyrad_dict_attr_index(dict, NULL, key.type, key.vendor_id, key.vendor_type, TRAD_NO)) < 0)
      return;
   attr = dict->attrs_type[attr_idx];

   printf("\n# %s Values\n", attr->name);
   for(pos = value_idx; ((pos < dict->values_data_len) && (!(tinyrad_dict_value_cmp_key_attr(&dict->values_data[pos], &key)))); pos++)
   {
      value = dict->values_data[pos];
      printf("VALUE         %-31s %-21s %" PRIu64 "\n", attr->name, value->name, value->data );
   };
   printf("\n");

   return;
}


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         size_t                        vendor_idx )
{
   size_t                  pos;
   ssize_t                 idx;
   char                    vendorstr[128];
   TinyRadDictVendor *     vendor;

   TinyRadDebugTrace();

   assert(dict       != NULL);
   assert(vendor_idx  < dict->vendors_id_len);

   vendor = dict->vendors_id[vendor_idx];

   for(pos = vendor_idx; ((pos > 0) && (dict->vendors_id[pos-1]->id == vendor->id)); pos--);
   vendor_idx = pos;

   snprintf(vendorstr, sizeof(vendorstr), "%s VSA's", vendor->name);
   printf("\n");
   printf("##############################################################################\n");
   printf("#                                                                            #\n");
   printf("#   %-70s   #\n", vendorstr);
   printf("#                                                                            #\n");
   printf("##############################################################################\n");
   printf("\n");

   // print vendor definition
   for(pos = vendor_idx; ((pos < dict->vendors_id_len) && (dict->vendors_id[pos]->id == vendor->id)); pos++)
   {
      if ( (vendor->len_octs != 1) || (vendor->type_octs != 1) )
         printf("VENDOR        %-31s %-21" PRIu32 " format=%" PRIu8 ",%" PRIu8 "\n", dict->vendors_id[pos]->name, vendor->id, vendor->type_octs, vendor->len_octs);
      else
         printf("VENDOR        %-31s %" PRIu32 "\n", dict->vendors_id[pos]->name, vendor->id);
   };
   printf("\n");

   if ((idx = tinyrad_dict_attr_index(dict, NULL, 26, vendor->id, 0, TRAD_YES)) < 0)
      return;

   printf("BEGIN-VENDOR  %s\n\n", vendor->name);
   for(pos = ((size_t)idx); ((pos < dict->attrs_type_len) && (__attr_vendor_id(dict->attrs_type[pos]) == vendor->id)); pos++)
      tinyrad_dict_print_attribute(dict, pos);
   printf("\nEND-VENDOR    %s\n\n", vendor->name);

   return;
}


//----------------------------//
// dictionary value functions //
//----------------------------//
#pragma mark dictionary value functions

uint8_t
__value_type(
         const TinyRadDictValue *      value )
{
   assert(value       != NULL);
   assert(value->attr != NULL);
   return( value->attr->type );
}


uint32_t
__value_vendor_id(
         const TinyRadDictValue *      value )
{
   assert(value       != NULL);
   assert(value->attr != NULL);
   return( ((value->attr->vendor)) ? value->attr->vendor->id : 0);
}


uint32_t
__value_vendor_type(
         const TinyRadDictValue *      value )
{
   assert(value       != NULL);
   assert(value->attr != NULL);
   return( ((value->attr->vendor)) ? value->attr->vendor_type : 0);
}


TinyRadDictValue *
tinyrad_dict_value_alloc(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr,
         const char *                  name,
         uint64_t                      data )
{
   TinyRadDictValue *   value;

   assert(dict != NULL);
   assert(attr != NULL);
   assert(name != NULL);

   if ((value = tinyrad_obj_alloc(sizeof(TinyRadDictValue), (void(*)(void*))&tinyrad_dict_value_free)) == NULL)
      return(NULL);

   if ((value->name = strdup(name)) == NULL)
   {
      tinyrad_dict_value_free(value);
      return(NULL);
   };

   value->attr = tinyrad_obj_retain(&attr->obj);

   dict->obj_count++;
   value->order   = dict->obj_count;
   value->data    = data;

   return(tinyrad_obj_retain(&value->obj));
}


int
tinyrad_dict_value_cmp_key_attr(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictValue * const *    obj = ptr;
   const TinyRadDictKey *              dat = key;

   if ((*obj)->attr->type != dat->type)
      return( ((*obj)->attr->type < dat->type) ? -1 : 1 );

   if (__value_vendor_id(*obj) != dat->vendor_id)
      return( (__value_vendor_id(*obj) < dat->vendor_id) ? -1 : 1 );

   if (__value_vendor_type(*obj) != dat->vendor_type)
      return( (__value_vendor_type(*obj) < dat->vendor_type) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_value_cmp_key_data(
         const void *                 ptr,
         const void *                 key )
{
   int                                 rc;
   const TinyRadDictValue * const *    obj = ptr;
   const TinyRadDictKey *              dat = key;

   if ((rc = tinyrad_dict_value_cmp_key_attr(ptr, key)) != 0)
      return(rc);

   if ((*obj)->data != dat->value_data)
      return( ((*obj)->data < dat->value_data) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_value_cmp_key_name(
         const void *                 ptr,
         const void *                 key )
{
   int                                 rc;
   const TinyRadDictValue * const *    obj = ptr;
   const TinyRadDictKey *              dat = key;

   if ((rc = tinyrad_dict_value_cmp_key_attr(ptr, key)) != 0)
      return(rc);

   return(strcasecmp( (*obj)->name, dat->str));
}


int
tinyrad_dict_value_cmp_obj_attr(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictValue * const * x = a;
   const TinyRadDictValue * const * y = b;

   // compare attribute type
   if ((*x)->attr->type != (*y)->attr->type)
      return( ((*x)->attr->type < (*y)->attr->type) ? -1 : 1 );

   // compare attribute vendor ID
   if (__value_vendor_id(*x) != __value_vendor_id(*y))
      return( (__value_vendor_id(*x) < __value_vendor_id(*y)) ? -1 : 1 );

   // compare attribute vendor type
   if (__value_vendor_type(*x) != __value_vendor_type(*y))
      return( (__value_vendor_type(*x) < __value_vendor_type(*y)) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_value_cmp_obj_data(
         const void *                 a,
         const void *                 b )
{
   int                                 rc;
   const TinyRadDictValue * const *    x = a;
   const TinyRadDictValue * const *    y = b;

   // compare attribute type, vendor id, and vendor type
   if ((rc = tinyrad_dict_value_cmp_obj_attr(a, b)) != 0)
      return(rc);

   // compare data of value
   if ((*x)->data != (*y)->data)
      return( ((*x)->data < (*y)->data) ? -1 : 1 );

   // compare definition order
   if ((*x)->order != (*y)->order)
      return( ((*x)->order < (*y)->order) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_value_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   int                                 rc;
   const TinyRadDictValue * const *    x = a;
   const TinyRadDictValue * const *    y = b;

   // compare attribute type, vendor id, and vendor type
   if ((rc = tinyrad_dict_value_cmp_obj_attr(a, b)) != 0)
      return(rc);

   // compare pointers
   if ( (!((*x)->name)) && (!((*y)->name)) )
      return(0);
   if (!((*x)->name))
      return(-1);
   if (!((*y)->name))
      return(1);

   // compare names
   return(strcasecmp( (*x)->name, (*y)->name));
}


void
tinyrad_dict_value_free(
         TinyRadDictValue *           value )
{
   TinyRadDebugTrace();

   if (!(value))
      return;

   if ((value->attr))
      tinyrad_obj_release(&value->attr->obj);

   if ((value->name))
      free(value->name);

   memset(value, 0, sizeof(TinyRadDictValue));
   free(value);

   return;
}


TinyRadDictValue *
tinyrad_dict_value_get(
         TinyRadDict *                 dict,
         const char *                  name,
         TinyRadDictAttr *             attr,
         uint8_t                       type,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type,
         uint64_t                      data )
{
   TinyRadDictValue * value;
   assert(dict != NULL);

   TinyRadDebugTrace();

   if (!(attr))
   {
      attr = tinyrad_dict_attr_lookup(dict, NULL, type, vendor_id, vendor_type);
      if (!(attr))
         return(NULL);
   };

   value = tinyrad_dict_value_lookup(dict, attr, name, data);

   return(tinyrad_obj_retain(&value->obj));
}


ssize_t
tinyrad_dict_value_index(
         TinyRadDict *                 dict,
         const char *                  name,
         uint8_t                       type,
         uint32_t                      vendor_id,
         uint32_t                      vendor_type,
         uint64_t                      value_data,
         int                           by_attr )
{
   size_t               width;
   size_t               len;
   unsigned             opts;
   TinyRadDictValue **  list;
   TinyRadDictKey       key;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert((by_attr == TRAD_YES) || (by_attr == TRAD_NO));

   width = sizeof(TinyRadDictAttr *);

   memset(&key, 0, sizeof(key));
   key.str           = name;
   key.type          = type;
   key.vendor_id     = vendor_id;
   key.vendor_type   = vendor_type;
   key.value_data    = value_data;

   if ((name))
   {
      len      = dict->values_name_len;
      list     = dict->values_name;
      compar   = &tinyrad_dict_value_cmp_key_name;
      opts     = TINYRAD_ARRAY_LASTDUP;
   } else {
      len      = dict->values_data_len;
      list     = dict->values_data;
      //                                 Lookup value by dattribute         Lookup value by data
      compar   = (by_attr == TRAD_YES) ? &tinyrad_dict_value_cmp_key_attr : &tinyrad_dict_value_cmp_key_data;
      opts     = (by_attr == TRAD_YES) ? TINYRAD_ARRAY_FIRSTDUP           : TINYRAD_ARRAY_LASTDUP;
   };

   return(tinyrad_array_search(list, len, width, &key, opts, NULL, compar));
}


int
tinyrad_dict_value_info(
         TinyRadDictValue *            value,
         int                           param,
         void *                        outvalue )
{
   unsigned       uval;
   TinyRadDebugTrace();

   assert(value     != NULL);
   assert(outvalue  != NULL);

   // get attribute options
   switch(param)
   {
      case TRAD_DICT_OPT_ATTR_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_ATTR_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", value->attr->name);
      if ( (*((char **)outvalue) = strdup(value->attr->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_DICT_OPT_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", value->name);
      if ( (*((char **)outvalue) = strdup(value->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_DICT_OPT_REF_COUNT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_REF_COUNT, outvalue )", __func__);
      uval = (unsigned)tinyrad_obj_retain_count(&value->obj);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %u", uval);
      *((unsigned *)outvalue) = uval;
      break;

      case TRAD_DICT_OPT_TYPE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_TYPE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", value->attr->type);
      *((uint8_t *)outvalue) = value->attr->type;
      break;

      case TRAD_DICT_OPT_DATA:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_DATA, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu64 "\"", value->data);
      *((uint64_t *)outvalue) = value->data;
      break;

      case TRAD_DICT_OPT_VEND_ID:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_ID, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", __value_vendor_id(value));
      *((uint32_t *)outvalue) = __value_vendor_id(value);
      break;

      case TRAD_DICT_OPT_VEND_TYPE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_TYPE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", __value_vendor_type(value));
      *((uint32_t *)outvalue) = __value_vendor_type(value);
      break;

      default:
      return(TRAD_EOPTERR);
   };

   return(TRAD_SUCCESS);
}


TinyRadDictValue *
tinyrad_dict_value_lookup(
         TinyRadDict *                dict,
         TinyRadDictAttr *            attr,
         const char *                 name,
         uint64_t                     data )
{
   size_t               width;
   size_t               len;
   unsigned             opts;
   TinyRadDictKey       key;
   TinyRadDictValue **  list;
   TinyRadDictValue **  res;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict   != NULL);
   assert(attr   != NULL);

   width    = sizeof(TinyRadDictValue *);
   opts     = TINYRAD_ARRAY_LASTDUP;

   memset(&key, 0, sizeof(key));
   key.str           = name;
   key.type          = attr->type;
   key.vendor_id     = __attr_vendor_id(attr);
   key.vendor_type   = attr->vendor_type;
   key.value_data    = data;

   if ((name))
   {
      len      = dict->values_name_len;
      list     = dict->values_name;
      compar   = &tinyrad_dict_value_cmp_key_name;
   } else {
      len      = dict->values_data_len;
      list     = dict->values_data;
      compar   = &tinyrad_dict_value_cmp_key_data;
   };

   res = tinyrad_array_get(list, len, width, &key, opts, compar);
   return( ((res)) ? *res : NULL);
}


//-----------------------------//
// dictionary vendor functions //
//-----------------------------//
#pragma mark dictionary vendor functions

TinyRadDictVendor *
tinyrad_dict_vendor_alloc(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     id,
         uint8_t                      type_octs,
         uint8_t                      len_octs )
{
   TinyRadDictVendor *     vendor;

   assert(dict != NULL);
   assert(name != NULL);

   if ((vendor = tinyrad_obj_alloc(sizeof(TinyRadDictVendor), (void(*)(void*))&tinyrad_dict_vendor_free)) == NULL)
      return(NULL);

   if ((vendor->name = strdup(name)) == NULL)
   {
      tinyrad_dict_vendor_free(vendor);
      return(NULL);
   };

   dict->obj_count++;
   vendor->order     = dict->obj_count;
   vendor->id        = id;
   vendor->type_octs = type_octs;
   vendor->len_octs  = len_octs;

   return(tinyrad_obj_retain(&vendor->obj));
}


int
tinyrad_dict_vendor_cmp_key_id(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictVendor * const *   obj = ptr;
   const uint32_t *                    dat = key;
   if ((*obj)->id == *dat)
      return(0);
   return( ((*obj)->id < *dat) ? -1 : 1 );
}


int
tinyrad_dict_vendor_cmp_key_name(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictVendor * const *   obj = ptr;
   const char *                        dat = key;
   return(strcasecmp( (*obj)->name, dat));
}


int
tinyrad_dict_vendor_cmp_obj_id(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictVendor * const * x = a;
   const TinyRadDictVendor * const * y = b;

   if ((*x)->id != (*y)->id)
      return( ((*x)->id < (*y)->id) ? -1 : 1 );

   if ((*x)->order != (*y)->order)
      return( ((*x)->order < (*y)->order) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_vendor_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictVendor * const * x = a;
   const TinyRadDictVendor * const * y = b;

   if ( (!((*x)->name)) && (!((*y)->name)) )
      return(0);

   if (!((*x)->name))
      return(-1);

   if (!((*y)->name))
      return(1);

   return(strcasecmp( (*x)->name, (*y)->name));
}


/// wrapper around stat() for dictionary processing
///
/// @param[in]  vendor        dictionar vendor reference
void
tinyrad_dict_vendor_free(
         TinyRadDictVendor *          vendor )
{
   TinyRadDebugTrace();

   if (!(vendor))
      return;

   if ((vendor->name))
      free(vendor->name);

   memset(vendor, 0, sizeof(TinyRadDictVendor));
   free(vendor);

   return;
}


TinyRadDictVendor *
tinyrad_dict_vendor_get(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     vendor_id )
{
   TinyRadDictVendor * vendor;
   assert(dict != NULL);
   if ((vendor = tinyrad_dict_vendor_lookup(dict, name, vendor_id)) == NULL)
      return(NULL);
   return(tinyrad_obj_retain(&vendor->obj));
}


int
tinyrad_dict_vendor_info(
         TinyRadDictVendor *           vendor,
         int                           param,
         void *                        outvalue )
{
   unsigned    uval;

   TinyRadDebugTrace();

   if ( (!(vendor)) || (!(outvalue)) )
      return(TRAD_EINVAL);

   // get attribute options
   switch(param)
   {
      case TRAD_DICT_OPT_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", vendor->name);
      if ( (*((char **)outvalue) = strdup(vendor->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_DICT_OPT_LEN_OCTS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_LEN_OCTS, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", vendor->len_octs);
      *((uint8_t *)outvalue) = vendor->len_octs;
      break;

      case TRAD_DICT_OPT_REF_COUNT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_REF_COUNT, outvalue )", __func__);
      uval = (unsigned)atomic_fetch_add(&vendor->obj.ref_count, 0);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %u", uval);
      *((unsigned *)outvalue) = uval;
      break;

      case TRAD_DICT_OPT_VEND_ID:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_ID, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", vendor->id);
      *((uint32_t *)outvalue) = vendor->id;
      break;

      case TRAD_DICT_OPT_TYPE_OCTS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_TYPE_OCTS, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", vendor->type_octs);
      *((uint8_t *)outvalue) = vendor->type_octs;
      break;

      default:
      return(TRAD_EINVAL);
   };

   return(TRAD_SUCCESS);
}


/// wrapper around stat() for dictionary processing
///
TinyRadDictVendor *
tinyrad_dict_vendor_lookup(
         TinyRadDict *                dict,
         const char *                 name,
         uint32_t                     id )
{
   void **                 list;
   const void *            key;
   size_t                  len;
   size_t                  width;
   unsigned                opts;
   TinyRadDictVendor **    res;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(dict   != NULL);

   width = sizeof(TinyRadDictVendor *);
   opts = TINYRAD_ARRAY_LASTDUP;

   if ((name))
   {
      list   = (void **)dict->vendors_name;
      len    = dict->vendors_name_len;
      key    = name;
      compar = &tinyrad_dict_vendor_cmp_key_name;
   } else {
      list   = (void **)dict->vendors_id;
      len    = dict->vendors_id_len;
      key    = &id;
      compar = &tinyrad_dict_vendor_cmp_key_id;
   };

   res = tinyrad_array_get(list, len, width, key, opts, compar);
   return( ((res)) ? *res : NULL);
}

/* end of source */
