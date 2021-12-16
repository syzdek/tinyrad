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

#include "lfile.h"
#include "lmemory.h"


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

void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr );


void
tinyrad_dict_vendor_destroy(
         TinyRadDictVendor *           vendor );


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
/// @param[in]  attr          reference to dictionary attribute
void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr )
{
   if (!(attr))
      return;
   if ((attr->name))
      free(attr->name);
   if ((attr->oid))
      free(attr->oid);
   bzero(attr, sizeof(TinyRadDictAttr));
   free(attr);
   return;
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

   // free attributes
   if ((dict->attrs))
   {
      for(pos = 0; (pos < dict->attrs_len); pos++)
         tinyrad_dict_attr_destroy(dict->attrs[pos]);
      free(dict->attrs);
   };

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
   TinyRadFile *         incl;
   int                   action;

   assert(dict != NULL);
   assert(path != NULL);

   if ((msgsp))
      *msgsp = NULL;

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
      if (!(file->argc))
      {
         parent = file->parent;
         tinyrad_file_destroy(file, TRAD_FILE_NORECURSE);
         file = parent;
         continue;
      };

      // interprets arguments
      if      (!(strcasecmp(file->argv[0], "$INCLUDE")))     action = TRAD_DICT_INCLUDE;
      else if (!(strcasecmp(file->argv[0], "ATTRIBUTE")))    action = TRAD_DICT_ATTRIBUTE;
      else if (!(strcasecmp(file->argv[0], "BEGIN-VENDOR"))) action = TRAD_DICT_BEGIN_VENDOR;
      else if (!(strcasecmp(file->argv[0], "END-VENDOR")))   action = TRAD_DICT_END_VENDOR;
      else if (!(strcasecmp(file->argv[0], "VALUE")))        action = TRAD_DICT_VALUE;
      else if (!(strcasecmp(file->argv[0], "VENDOR")))       action = TRAD_DICT_VENDOR;
      else
      {
         tinyrad_file_error(file, TRAD_ESYNTAX, msgsp);
         tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
         return(TRAD_ESYNTAX);
      };

      // perform requested action
      switch(action)
      {
         case TRAD_DICT_INCLUDE:
         if ((rc = tinyrad_file_init(&incl, file->argv[1], dict->paths, file)) != TRAD_SUCCESS)
         {
            file = ((incl)) ? incl : file;
            tinyrad_file_error(file, rc, msgsp);
            tinyrad_file_destroy(file, TRAD_FILE_RECURSE);
            return(rc);
         };
         file = incl;
         break;

         default:
printf("%s: %3i: ----", "dict", file->line);
for(rc = 0; (rc < (int)file->argc); rc++)
   printf(" %s", file->argv[rc]);
printf("\n");
         break;
      };
   };

   tinyrad_file_error(NULL, TRAD_SUCCESS, msgsp);
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
   dict->opts = opts;

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
   size_t      low;
   size_t      mid;
   size_t      high;

   assert(idx  != NULL);

   if (!(list))
      return(NULL);
   if (!(len))
      return(NULL);

   low  = 0;
   high = (len - 1);
   mid  = len / 2;

   while(mid > low)
   {
      if ((rc = (*compar)(list[mid], idx)) == 0)
         return(list[mid]);
      else if (rc < 0)
         high = mid;
      else
         low = mid;
      mid = (high + low) / 2;
   };

   if ((rc = (*compar)(list[mid], idx)) == 0)
      return(list[mid]);
   if ((rc = (*compar)(list[high], idx)) == 0)
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

   for(pos = 0; (pos < dict->vendors_len); pos++)
      printf("VENDOR %s %" PRIu32 "\n", dict->vendors[pos]->name, dict->vendors[pos]->id);

   printf("# end of processed dictionary\n");

   return;
}


/// wrapper around stat() for dictionary processing
///
/// @param[out] vendorp       dictionar vendor reference
int
tinyrad_dict_vendor_add(
         TinyRadDict *                dict,
         TinyRadDictVendor **         vendorp,
         const char *                 name,
         uint32_t                     id )
{
   size_t               size;
   size_t               vendors_len;
   void *               ptr;
   TinyRadDictVendor *  vendor;

   assert(dict   != NULL);
   assert(name   != NULL);

   // verify attribute doesn't exist
   if ((tinyrad_dict_vendor_lookup(dict, name, 0)))
      return(TRAD_EEXISTS);
   if ((tinyrad_dict_vendor_lookup(dict, NULL, id)))
      return(TRAD_EEXISTS);

   // resize vendor lists
   vendors_len = dict->vendors_len + 1;
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
   vendor->type_octs = 1;
   vendor->len_octs  = 1;

   // copy vendor name
   if ((vendor->name = strdup(name)) == NULL)
   {
      tinyrad_dict_vendor_destroy(vendor);
      return(TRAD_ENOMEM);
   };

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

   return( (int)(((int64_t)vendor1->id) - ((int64_t)vendor1->id)) );
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
   if ((vendor->name))
      free(vendor->name);
   bzero(vendor, sizeof(TinyRadDictVendor));
   free(vendor);
   return;
}


/* end of source */
