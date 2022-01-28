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

typedef struct _tinyrad_dict_attr_key TinyRadDictAttrType;

struct _tinyrad_dict_attr_key
{
   uint32_t              type;
   uint32_t              vendor_id;
   uint32_t              vendor_type;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//---------------------------//
// dictionary misc functions //
//---------------------------//
#pragma mark dictionary misc functions


//--------------------------------//
// dictionary attribute functions //
//--------------------------------//
#pragma mark dictionary attribute functions

int
tinyrad_dict_attr_add(
         TinyRadDict *                 dict,
         TinyRadDictAttr **            attrp,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_type,
         uint8_t                       datatype,
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
tinyrad_dict_attr_cmp_obj_name(
         const void *                 a,
         const void *                 b );


int
tinyrad_dict_attr_cmp_obj_type(
         const void *                 a,
         const void *                 b );


//----------------------------//
// dictionary import functions //
//----------------------------//
#pragma mark dictionary import functions


//----------------------------//
// dictionary parse functions //
//----------------------------//
#pragma mark dictionary parse functions

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


//----------------------------//
// dictionary print functions //
//----------------------------//
#pragma mark dictionary print functions

void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr );


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor );


//----------------------------//
// dictionary value functions //
//----------------------------//
#pragma mark dictionary value functions

int
tinyrad_dict_value_add(
         TinyRadDictAttr *             attr,
         TinyRadDictValue **           valuep,
         const char *                  name,
         uint64_t                      numeral );


int
tinyrad_dict_value_cmp_key_name(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_value_cmp_obj_name(
         const void *                  a,
         const void *                  b );


int
tinyrad_dict_value_cmp_key_data(
         const void *                 ptr,
         const void *                 key );


int
tinyrad_dict_value_cmp_obj_value(
         const void *                  a,
         const void *                  b );


void
tinyrad_dict_value_destroy(
         TinyRadDictValue *           value );


//-----------------------------//
// dictionary vendor functions //
//-----------------------------//
#pragma mark dictionary vendor functions

int
tinyrad_dict_vendor_add(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
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


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_dict_data_type[]
static const TinyRadMap tinyrad_dict_data_type[] =
{
   { "ABINARY",      TRAD_ABINARY},
   { "BYTE",         TRAD_BYTE},
   { "DATE",         TRAD_DATE},
   { "ETHER",        TRAD_ETHER},
   { "EXTENDED",     TRAD_EXTENDED},
   { "IFID",         TRAD_IFID},
   { "INTEGER",      TRAD_INTEGER},
   { "INTEGER64",    TRAD_INTEGER64},
   { "IPADDR",       TRAD_IPADDR},
   { "IPV4PREFIX",   TRAD_IPV4PREFIX},
   { "IPV6ADDR",     TRAD_IPV6ADDR},
   { "IPV6PREFIX",   TRAD_IPV6PREFIX},
   { "LONGEXTENDED", TRAD_LONGEXTENDED},
   { "OCTETS",       TRAD_OCTETS},
   { "RAD_TLV",      TRAD_TLV},
   { "SHORT",        TRAD_SHORT},
   { "SIGNED",       TRAD_SIGNED},
   { "STRING",       TRAD_STRING },
   { "VSA",          TRAD_VSA },
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


/// Initialize dicitionary file buffer
///
/// @param[in]  dict          dictionary reference
void
tinyrad_dict_destroy(
         TinyRadDict *                dict )
{
   size_t        pos;

   TinyRadDebugTrace();

   if (!(dict))
      return;

   if (atomic_fetch_sub(&dict->ref_count, 1) > 1)
      return;

   // free attributes
   if ((dict->attrs_name))
   {
      for(pos = 0; (pos < dict->attrs_name_len); pos++)
         tinyrad_dict_attr_destroy(dict->attrs_name[pos]);
      free(dict->attrs_name);
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
   if ((dict->vendors_name))
   {
      for(pos = 0; (pos < dict->vendors_name_len); pos++)
         tinyrad_dict_vendor_destroy(dict->vendors_name[pos]);
      free(dict->vendors_name);
   };
   if ((dict->vendors_id))
      free(dict->vendors_id);

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

   if (!(dict = malloc(sizeof(TinyRadDict))))
      return(-1);
   memset(dict, 0, sizeof(TinyRadDict));
   dict->opts      = opts;
   atomic_init(&dict->ref_count, 1);

   // initializes paths
   if ((dict->paths = malloc(sizeof(char *))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->paths[0] = NULL;

   *dictp = dict;

   return(TRAD_SUCCESS);
}


//--------------------------------//
// dictionary attribute functions //
//--------------------------------//
#pragma mark dictionary attribute functions


/// Destroys and frees resources of a RADIUS dictionary attribute
///
/// @param[out] attrp         reference to dictionary attribute
int
tinyrad_dict_attr_add(
         TinyRadDict *                 dict,
         TinyRadDictAttr **            attrp,
         const char *                  name,
         uint8_t                       type,
         TinyRadDictVendor *           vendor,
         uint32_t                      vendor_type,
         uint8_t                       datatype,
         uint32_t                      flags )
{
   uint32_t             vendor_id;
   size_t               size;
   size_t               width;
   void *               ptr;
   TinyRadDictAttr *    attr;
   ssize_t              rc;
   unsigned             opts;

   TinyRadDebugTrace();

   assert(dict      != NULL);
   assert(name      != NULL);
   assert(type      != 0);
   assert(datatype  != 0);

   vendor_id   = ((vendor)) ? vendor->id  : 0;
   vendor_type = ((vendor)) ? vendor_type : 0;

   // verify attribute doesn't exist
   if ((attr = tinyrad_dict_attr_lookup(dict, name, 0, 0, 0)) != NULL)
   {
      if ( (attr->flags|TRAD_DFLT_ATTR) != (flags|TRAD_DFLT_ATTR) )
         return(TRAD_EEXISTS);
      if (attr->type != type)
         return(TRAD_EEXISTS);
      if (attr->vendor_id != vendor_id)
         return(TRAD_EEXISTS);
      if (attr->vendor_type != vendor_type)
         return(TRAD_EEXISTS);
      if (attr->data_type != datatype)
         return(TRAD_EEXISTS);
      if ((attrp))
         *attrp = attr;
      return(TRAD_SUCCESS);
   };
   if ((tinyrad_dict_attr_lookup(dict, NULL, type, vendor_id, vendor_type)))
      return(TRAD_EEXISTS);

   // resize attribute lists in dictionary
   size = sizeof(TinyRadDictAttr *) * (dict->attrs_name_len+1);
   if ((ptr = realloc(dict->attrs_name, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->attrs_name = ptr;
   size = sizeof(TinyRadDictAttr *) * (dict->attrs_type_len+1);
   if ((ptr = realloc(dict->attrs_type, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->attrs_type = ptr;

   // resize attribute lists in vendor
   if ((vendor))
   {
      size = sizeof(TinyRadDictAttr *) * (vendor->attrs_name_len+1);
      if ((ptr = realloc(vendor->attrs_name, size)) == NULL)
         return(TRAD_ENOMEM);
      vendor->attrs_name = ptr;
      size = sizeof(TinyRadDictAttr *) * (vendor->attrs_type_len+1);
      if ((ptr = realloc(vendor->attrs_type, size)) == NULL)
         return(TRAD_ENOMEM);
      vendor->attrs_type = ptr;
   };

   // allocate memory
   if ((attr = malloc(sizeof(TinyRadDictAttr))) == NULL)
      return(TRAD_ENOMEM);
   memset(attr, 0, sizeof(TinyRadDictAttr));
   atomic_init(&attr->ref_count, 1);
   attr->type        = type;
   attr->vendor_id   = vendor_id;
   attr->vendor_type = vendor_type;
   attr->data_type   = datatype;
   attr->flags       = flags;
   if ((attr->name = strdup(name)) == NULL)
   {
      tinyrad_dict_attr_destroy(attr);
      return(TRAD_ENOMEM);
   };

   width = sizeof(TinyRadDictAttr *);
   opts  = TINYRAD_ARRAY_INSERT | TINYRAD_ARRAY_LASTDUP;

   // save attribute to dictionary
   if ((rc = tinyrad_array_add((void **)&dict->attrs_name, &dict->attrs_name_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_name, NULL, NULL)) < 0)
   {
      tinyrad_dict_attr_destroy(attr);
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   };
   if ((rc = tinyrad_array_add((void **)&dict->attrs_type, &dict->attrs_type_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_type, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);

   if (!(vendor))
   {
      if ((attrp))
         *attrp = attr;
      return(TRAD_SUCCESS);
   };

   // add vendor metadata
   attr->vendor_id   = vendor->id;
   attr->type_octs   = vendor->type_octs;
   attr->len_octs    = vendor->len_octs;
   atomic_fetch_add(&attr->ref_count, 1);

   // save attribute to vendor
   if ((rc = tinyrad_array_add((void **)&vendor->attrs_name, &vendor->attrs_name_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_name, NULL, NULL)) < 0)
   {
      tinyrad_dict_attr_destroy(attr);
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   };
   if ((rc = tinyrad_array_add((void **)&vendor->attrs_type, &vendor->attrs_type_len, width, &attr, opts, &tinyrad_dict_attr_cmp_obj_type, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);

   if ((attrp))
      *attrp = attr;

   return(TRAD_SUCCESS);
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
   const TinyRadDictAttrType *         dat = key;

   if ((*obj)->type != dat->type)
      return( ((*obj)->type < dat->type) ? -1 : 1 );

   if ((*obj)->vendor_id != dat->vendor_id)
      return( ((*obj)->vendor_id < dat->vendor_id) ? -1 : 1 );

   if ((*obj)->vendor_type != dat->vendor_type)
      return( ((*obj)->vendor_type < dat->vendor_type) ? -1 : 1 );

   return(0);
}


int
tinyrad_dict_attr_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictAttr * const * x = a;
   const TinyRadDictAttr * const * y = b;
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

   if ((*x)->vendor_id != (*y)->vendor_id)
      return( ((*x)->vendor_id < (*y)->vendor_id) ? -1 : 1 );

   if ((*x)->vendor_type != (*y)->vendor_type)
      return( ((*x)->vendor_type < (*y)->vendor_type) ? -1 : 1 );

   return(0);
}


/// Destroys and frees resources of a RADIUS dictionary attribute
///
/// @param[in]  attr          reference to dictionary attribute
void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr )
{
   size_t   pos;
   TinyRadDebugTrace();
   if (!(attr))
      return;
   if (atomic_fetch_sub(&attr->ref_count, 1) > 1)
      return;
   if ((attr->name))
      free(attr->name);
   for(pos = 0; (pos < attr->values_name_len); pos++)
      tinyrad_dict_value_destroy(attr->values_name[pos]);
   if ((attr->values_name))
      free(attr->values_name);
   if ((attr->values_numeric))
      free(attr->values_numeric);
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
   if ((attr = tinyrad_dict_attr_lookup(dict, name, type, vendor_id, vendor_type)) == NULL)
      return(NULL);
   atomic_fetch_add(&attr->ref_count, 1);
   return(attr);
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
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", attr->len_octs);
      *((uint8_t *)outvalue) = attr->len_octs;
      break;

      case TRAD_DICT_OPT_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", attr->name);
      if ( (*((char **)outvalue) = strdup(attr->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_DICT_OPT_REF_COUNT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_REF_COUNT, outvalue )", __func__);
      uval = (unsigned)atomic_fetch_add(&attr->ref_count, 0);
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
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", attr->type_octs);
      *((uint8_t *)outvalue) = attr->type_octs;
      break;

      case TRAD_DICT_OPT_VEND_ID:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_ID, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", attr->vendor_id);
      *((uint32_t *)outvalue) = attr->vendor_id;
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
   TinyRadDictAttrType  attr_type;
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
   uint8_t                 datatype;
   uint32_t                flags;
   uint64_t                data;
   uint32_t                vendor_id;
   uint32_t                vendor_type;
   uint8_t                 type_octs;
   uint8_t                 len_octs;
   const char *            attr_name;
   const char *            value_name;
   const char *            vendor_name;
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
         if ((rc = tinyrad_dict_vendor_add(dict, NULL, vendor_name, vendor_id, type_octs, len_octs)) != TRAD_SUCCESS)
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
         datatype       = (uint8_t)attr_defs[pos].data_type;
         flags          = (uint32_t)attr_defs[pos].flags;
         flags         |= TRAD_DFLT_ATTR;
         vendor         = tinyrad_dict_vendor_lookup(dict, NULL, vendor_id);
         assert( ((vendor)) || (!(vendor_id)) );
         if ((rc = tinyrad_dict_attr_add(dict, NULL, attr_name, type, vendor, vendor_type, datatype, flags)) != TRAD_SUCCESS)
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
         if ((rc = tinyrad_dict_value_add(attr, NULL, value_name, data)) != TRAD_SUCCESS)
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
   uint8_t      datatype;
   uint8_t     type;
   uint32_t    flags;
   uint32_t    flag;
   uint32_t    vendor_type;
   int         rc;
   char *      ptr;
   char *      str;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if ( (file->argc < 4) || (file->argc > 5) )
      return(TRAD_ESYNTAX);
   if ((datatype = (uint8_t)tinyrad_map_lookup_name(tinyrad_dict_data_type, file->argv[3], NULL)) == 0)
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

   if ((rc = tinyrad_dict_attr_add(dict, NULL, file->argv[1], type, vendor, vendor_type, datatype, flags)) != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
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
   TinyRadDictAttr *  attr;
   uint64_t           number;
   int                rc;
   char *             ptr;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   if (file->argc == 3)
      return(TRAD_ESYNTAX);
   if ((attr = tinyrad_dict_attr_lookup(dict, file->argv[1], 0, 0, 0)) == NULL)
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
tinyrad_dict_parse_vendor(
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


//----------------------------//
// dictionary print functions //
//----------------------------//
#pragma mark dictionary print functions

void
tinyrad_dict_print(
         TinyRadDict *                 dict,
         uint32_t                      opts )
{
   size_t     pos;

   TinyRadDebugTrace();

   assert(dict   != NULL);
   assert(opts   != 0);

   printf("# processed dictionary\n");

   for(pos = 0; (pos < dict->attrs_type_len); pos ++)
      if (!(dict->attrs_type[pos]->vendor_id))
         tinyrad_dict_print_attribute(dict, dict->attrs_type[pos]);

   for(pos = 0; (pos < dict->vendors_name_len); pos++)
      tinyrad_dict_print_vendor(dict, dict->vendors_name[pos]);

   printf("# end of processed dictionary\n");

   return;
}


void
tinyrad_dict_print_attribute(
         TinyRadDict *                 dict,
         TinyRadDictAttr *             attr )
{
   size_t              pos;
   size_t              flags;
   size_t              values;
   char                flagstr[128];
   char                datatype[32];
   const char *        str;
   TinyRadDictValue *  value;

   TinyRadDebugTrace();

   assert(dict != NULL);
   assert(attr != NULL);

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

   printf("ATTRIBUTE     %-31s %-21" PRIu32 " %s%s\n", attr->name, attr->type, datatype, flagstr);
   values = 0;
   for(pos = 0; (pos < attr->values_numeric_len); pos++)
   {
      if (!(values))
         printf("\n# %s Values\n", attr->name);
      value = attr->values_numeric[pos];
      printf("VALUE         %-31s %-21s %" PRIu64 "\n", attr->name, value->name, value->data );
      values++;
   };
   if ((values))
      printf("\n");

   return;
}


void
tinyrad_dict_print_vendor(
         TinyRadDict *                 dict,
         TinyRadDictVendor *           vendor )
{
   size_t pos;
   char   flagstr[128];

   TinyRadDebugTrace();

   assert(dict   != NULL);
   assert(vendor != NULL);

   flagstr[0] = '\0';
   if ( (vendor->len_octs != 1) || (vendor->type_octs != 1) )
      snprintf(flagstr, sizeof(flagstr), "format=%" PRIu8 ",%" PRIu8, vendor->type_octs, vendor->len_octs);

   if ((flagstr[0]))
      printf("\nVENDOR        %-31s %-21" PRIu32 " %s\n\n", vendor->name, vendor->id, flagstr);
   else
      printf("\nVENDOR        %-31s %" PRIu32 "\n\n", vendor->name, vendor->id);

   if (!(vendor->attrs_type_len))
      return;

   printf("BEGIN-VENDOR  %s\n\n", vendor->name);
   for(pos = 0; (pos < vendor->attrs_type_len); pos++)
      tinyrad_dict_print_attribute(dict, vendor->attrs_type[pos]);
   printf("\nEND-VENDOR    %s\n\n", vendor->name);

   return;
}


//----------------------------//
// dictionary value functions //
//----------------------------//
#pragma mark dictionary value functions

int
tinyrad_dict_value_add(
         TinyRadDictAttr *             attr,
         TinyRadDictValue **           valuep,
         const char *                  name,
         uint64_t                      numeral )
{
   TinyRadDictValue *   value;
   size_t               size;
   size_t               width;
   ssize_t              rc;
   void *               ptr;
   unsigned             opts;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(attr      != NULL);
   assert(name      != NULL);

    // increase size of lists
   size = sizeof(TinyRadDictValue *) * (attr->values_name_len+1);
   if ((ptr = realloc(attr->values_name, size)) == NULL)
      return(TRAD_ENOMEM);
   attr->values_name = ptr;
   size = sizeof(TinyRadDictValue *) * (attr->values_numeric_len+1);
   if ((ptr = realloc(attr->values_numeric, size)) == NULL)
      return(TRAD_ENOMEM);
   attr->values_numeric = ptr;

   // allocate new value
   if ((value = malloc(sizeof(TinyRadDictValue))) == NULL)
      return(TRAD_ENOMEM);
   memset(value, 0, sizeof(TinyRadDictValue));
   value->data = numeral;
   if ((value->name = strdup(name)) == NULL)
   {
      tinyrad_dict_value_destroy(value);
      return(TRAD_ENOMEM);
   };

   // save value by name
   opts     = TINYRAD_ARRAY_INSERT;
   width    = sizeof(TinyRadDictValue *);
   compar   = &tinyrad_dict_value_cmp_obj_name;
   if ((rc = tinyrad_array_add((void **)&attr->values_name, &attr->values_name_len, width, &value, opts, compar, NULL, NULL)) < 0)
   {
      tinyrad_dict_value_destroy(value);
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   };

   // save value by value
   opts     = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_LASTDUP;
   width    = sizeof(TinyRadDictValue *);
   compar   = &tinyrad_dict_value_cmp_obj_value;
   if ((rc = tinyrad_array_add((void **)&attr->values_numeric, &attr->values_numeric_len, width, &value, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);

   if ((valuep))
      *valuep = value;

   return(TRAD_SUCCESS);
}


int
tinyrad_dict_value_cmp_key_name(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictValue * const * obj = ptr;
   const char *                     dat = key;
   return(strcasecmp( (*obj)->name, dat));
}


int
tinyrad_dict_value_cmp_key_data(
         const void *                 ptr,
         const void *                 key )
{
   const TinyRadDictValue * const * obj = ptr;
   const uint64_t *                 dat = key;
   if ((*obj)->data == *dat)
      return(0);
   return( ((*obj)->data < *dat) ? -1 : 1 );
}


int
tinyrad_dict_value_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictValue * const * x = a;
   const TinyRadDictValue * const * y = b;
   return(strcasecmp( (*x)->name, (*y)->name));
}


int
tinyrad_dict_value_cmp_obj_value(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictValue * const * x = a;
   const TinyRadDictValue * const * y = b;
   if ((*x)->data == (*y)->data)
      return(0);
   return( ((*x)->data < (*y)->data) ? -1 : 1 );
}


void
tinyrad_dict_value_destroy(
         TinyRadDictValue *           value )
{
   TinyRadDebugTrace();

   if (!(value))
      return;

   if ((value->name))
      free(value->name);

   memset(value, 0, sizeof(TinyRadDictValue));
   free(value);

   return;
}


int
tinyrad_dict_value_info(
         TinyRadDictValue *            value,
         int                           param,
         void *                        outvalue )
{
   TinyRadDebugTrace();

   assert(value     != NULL);
   assert(outvalue  != NULL);

   // get attribute options
   switch(param)
   {
      case TRAD_DICT_OPT_NAME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_NAME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"%s\"", value->name);
      if ( (*((char **)outvalue) = strdup(value->name)) == NULL)
         return(TRAD_ENOMEM);
      break;

      //case TRAD_DICT_OPT_REF_COUNT:
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_REF_COUNT, outvalue )", __func__);
      //ival = (unsigned)atomic_fetch_add(&value->ref_count, 0);
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %u", uval);
      //*((unsigned *)outvalue) = uval;
      //break;

      //case TRAD_DICT_OPT_TYPE:
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_TYPE, outvalue )", __func__);
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu8 "\"", value->type);
      //*((uint8_t *)outvalue) = value->type;
      //break;

      case TRAD_DICT_OPT_DATA:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_DATA, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu64 "\"", value->data);
      *((uint64_t *)outvalue) = value->data;
      break;

      //case TRAD_DICT_OPT_VEND_ID:
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_ID, outvalue )", __func__);
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", value->vendor_id);
      //*((uint32_t *)outvalue) = value->vendor_id;
      //break;

      //case TRAD_DICT_OPT_VEND_TYPE:
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( attr, TRAD_DICT_OPT_VEND_TYPE, outvalue )", __func__);
      //TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: \"" PRIu32 "\"", value->vendor_type);
      //*((uint32_t *)outvalue) = value->vendor_type;
      //break;

      default:
      return(TRAD_EOPTERR);
   };

   return(TRAD_SUCCESS);
}


TinyRadDictValue *
tinyrad_dict_value_lookup(
         TinyRadDictAttr *            attr,
         const char *                 name,
         uint64_t                     num )
{
   size_t               width;
   size_t               len;
   unsigned             opts;
   const void *         key;
   TinyRadDictValue **  list;
   TinyRadDictValue **  res;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

   assert(attr   != NULL);

   width    = sizeof(TinyRadDictValue *);
   opts     = TINYRAD_ARRAY_LASTDUP;

   if ((name))
   {
      key      = name;
      len      = attr->values_name_len;
      list     = attr->values_name;
      compar   = &tinyrad_dict_value_cmp_key_name;
   } else {
      key      = &num;
      len      = attr->values_numeric_len;
      list     = attr->values_numeric;
      compar   = &tinyrad_dict_value_cmp_key_data;
   };

   res = tinyrad_array_get(list, len, width, key, opts, compar);
   return( ((res)) ? *res : NULL);
}


//-----------------------------//
// dictionary vendor functions //
//-----------------------------//
#pragma mark dictionary vendor functions

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
   size_t               width;
   ssize_t              rc;
   unsigned             opts;
   void *               ptr;
   TinyRadDictVendor *  vendor;
   int (*compar)(const void *, const void *);

   TinyRadDebugTrace();

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
   size = sizeof(TinyRadDictVendor *) * (dict->vendors_name_len + 2);
   if ((ptr = realloc(dict->vendors_name, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors_name = ptr;
   size = sizeof(TinyRadDictVendor *) * (dict->vendors_id_len + 2);
   if ((ptr = realloc(dict->vendors_id, size)) == NULL)
      return(TRAD_ENOMEM);
   dict->vendors_id = ptr;

   // initialize vendor
   if ((vendor = malloc(sizeof(TinyRadDictVendor))) == NULL)
      return(TRAD_ENOMEM);
   memset(vendor, 0, sizeof(TinyRadDictVendor));
   vendor->id        = id;
   vendor->type_octs = type_octs;
   vendor->len_octs  = len_octs;
   atomic_init(&vendor->ref_count, 1);
   if ((vendor->name = strdup(name)) == NULL)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return(TRAD_ENOMEM);
   };

   // save value by name
   opts     = TINYRAD_ARRAY_INSERT;
   width    = sizeof(TinyRadDictVendor *);
   compar   = &tinyrad_dict_vendor_cmp_obj_name;
   if ((rc = tinyrad_array_add((void **)&dict->vendors_name, &dict->vendors_name_len, width, &vendor, opts, compar, NULL, NULL)) < 0)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);
   };

   // save value by id
   opts     = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_LASTDUP;
   width    = sizeof(TinyRadDictVendor *);
   compar   = &tinyrad_dict_vendor_cmp_obj_id;
   if ((rc = tinyrad_array_add((void **)&dict->vendors_id, &dict->vendors_id_len, width, &vendor, opts, compar, NULL, NULL)) < 0)
      return( (rc == -2) ? TRAD_ENOMEM : TRAD_EEXISTS);

   if ((vendorp))
      *vendorp = vendor;

   return(0);
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
   if ((*x)->id == (*y)->id)
      return(0);
   return( ((*x)->id < (*y)->id) ? -1 : 1 );
}


int
tinyrad_dict_vendor_cmp_obj_name(
         const void *                 a,
         const void *                 b )
{
   const TinyRadDictVendor * const * x = a;
   const TinyRadDictVendor * const * y = b;
   return(strcasecmp( (*x)->name, (*y)->name));
}


/// wrapper around stat() for dictionary processing
///
/// @param[in]  vendor        dictionar vendor reference
void
tinyrad_dict_vendor_destroy(
         TinyRadDictVendor *          vendor )
{
   size_t   pos;

   TinyRadDebugTrace();

   if (!(vendor))
      return;

   if (atomic_fetch_sub(&vendor->ref_count, 1) > 1)
      return;

   if ((vendor->name))
      free(vendor->name);

   for(pos = 0; (pos < vendor->attrs_name_len); pos++)
      tinyrad_dict_attr_destroy(vendor->attrs_name[pos]);
   if ((vendor->attrs_name))
      free(vendor->attrs_name);
   if ((vendor->attrs_type))
      free(vendor->attrs_type);

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
   atomic_fetch_add(&vendor->ref_count, 1);
   return(vendor);
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
      uval = (unsigned)atomic_fetch_add(&vendor->ref_count, 0);
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
