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


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

void
tinyrad_dict_attr_destroy(
         TinyRadDictAttr *             attr );


int
tinyrad_dict_parse(
         TinyRadDict *                 dict,
         TinyRadFile *                 buff,
         uint32_t                      opts );


void
tinyrad_dict_vend_destroy(
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

   if ((dict->attrs))
   {
      for(pos = 0; (pos < dict->attrs_len); pos++)
         tinyrad_dict_attr_destroy(dict->attrs[pos]);
      free(dict->attrs);
   };

   if ((dict->paths))
   {
      for(pos = 0; (pos < dict->paths_len); pos++)
         free(dict->paths[pos]);
      free(dict->paths);
   };

   if ((dict->vendors))
   {
      for(pos = 0; (pos < dict->vendors_len); pos++)
         tinyrad_dict_vend_destroy(dict->vendors[pos]);
      free(dict->vendors);
   };

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
   size_t                pos;
   size_t                x;
   size_t                size;
   ssize_t               len;
   TinyRadFile *     buff;
   TinyRadFile *     parent;
   
   assert(dict != NULL);
   assert(path != NULL);

   if ((msgsp))
      *msgsp = NULL;

   // initialize buffer
   if ((rc = tinyrad_file_init(&buff, path, (const char **)dict->paths, NULL)) != TRAD_SUCCESS)
      return(rc);

   while((buff))
   {
      // process buffer
      for(pos = buff->pos; (pos < buff->len); pos++)
      {
         if (buff->data[pos] == '\n')
         {
            buff->line++;
            buff->data[pos] = '\0';
            if ((rc = tinyrad_dict_parse(dict, buff, opts)) != TRAD_SUCCESS)
            {
               while((buff))
               {
                  parent = buff->parent;
                  tinyrad_file_destroy(buff);
                  buff = parent;
               };
               return(rc);
            };
            buff->pos = pos + 1;
            if (buff->argc == 0)
               continue;
printf("%s: %3i: ----", "tinyrad_dict_import", buff->line);
for(rc = 0; (rc < (int)buff->argc); rc++)
   printf(" %s", buff->argv[rc]);
printf("\n");
         };
      };

      // shift unprocessed data to start of  buffer
      for(x = 0; (x <= (buff->len - buff->pos)); x++)
         buff->data[x] = buff->data[x + buff->pos];
      buff->len             -= buff->pos;
      buff->pos              = 0;

      // attempt to fill buffer
      size = sizeof(buff->data) - buff->len - 1;
      switch (len = read(buff->fd, &buff->data[buff->len], size))
      {
         case -1:
         while((buff))
         {
            parent = buff->parent;
            tinyrad_file_destroy(buff);
            buff = parent;
         };
         return(TRAD_EUNKNOWN);

         case 0:
         parent = buff->parent;
         tinyrad_file_destroy(buff);
         buff = parent;
         break;

         default:
         buff->len             += len;
         buff->data[buff->len]  = '\0';
         break;
      };
   };

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

   if ((dict->paths = malloc(sizeof(char **))) == NULL)
   {
      tinyrad_dict_destroy(dict);
      return(-1);
   };
   dict->paths[0] = NULL;

   *dictp = dict;

   return(TRAD_SUCCESS);
}


/// splits line into argv and argc style elements
///
/// @param[in]  dict          dictionary reference
/// @param[in]  buff          file buffer
/// @param[in]  opts          dictionary options
/// @return returns error code
int
tinyrad_dict_parse(
         TinyRadDict *                dict,
         TinyRadFile *                buff,
         uint32_t                     opts )
{
   size_t  pos;

   assert(dict != NULL);
   assert(buff != NULL);
   assert(opts == 0);

   buff->argc = 0;
   pos        = buff->pos;

   // process line
   while ((buff->data[pos] != '\0') && (buff->argc < TRAD_ARGV_SIZE))
   {
      switch(buff->data[pos])
      {
         case ' ':
         case '\t':
         break;

         case '#':
         case '\0':
         buff->data[pos] = '\0';
         return(TRAD_SUCCESS);

         default:
         buff->argv[buff->argc] = &buff->data[pos];
         buff->argc++;
         while (buff->data[pos] != '\0')
         {
            pos++;
            switch(buff->data[pos])
            {
               case '\0':
               case '#':
               return(TRAD_SUCCESS);

               case ' ':
               case '\t':
               buff->data[pos] = '\0';
               break;

               default:
               if (buff->data[pos] < '$')
                  return(TRAD_EUNKNOWN);
               if (buff->data[pos] > 'z')
                  return(TRAD_EUNKNOWN);
               break;
            };
         };
         break;
      };

      pos++;
   };

   return(TRAD_SUCCESS);
}


/// wrapper around stat() for dictionary processing
///
/// @param[in]  vendor        dictionar vendor reference
void
tinyrad_dict_vend_destroy(
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
