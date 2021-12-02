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
#define _LIB_LIBTINYRAD_LFILE_C 1
#include "lfile.h"


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

#include "lmemory.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

/// Destroys and frees resources of a RADIUS dictionary buffer
///
/// @param[in]  file          dictionary buffer reference
void
tinyrad_file_destroy(
         TinyRadFile *                 file)
{
   if (!(file))
      return;
   if ((file->path))
      free(file->path);
   if (file->fd != -1)
      close(file->fd);
   bzero(file, sizeof(TinyRadFile));
   free(file);
   return;
}


/// Add directory to search path for dictionary files
///
/// @param[in]  file          dictionary reference
/// @param[in]  errnum        string contain directory name
/// @param[out] msgsp         string contain directory name
/// @return returns error code
int
tinyrad_file_error(
         TinyRadFile *                 file,
         int                           errnum,
         char ***                      msgsp )
{
   int    rc;
   char   err[128];
   char   msg[256];

   if (msgsp == NULL)
      return(errnum);
   if (file == NULL)
      return(errnum);

   // reset error
   tinyrad_strerror_r(errnum, err, sizeof(err));
   tinyrad_strings_free(*msgsp);
   *msgsp = NULL;

   // record error
   snprintf(msg, sizeof(msg), "%s: %i: %s", file->path, file->line, err);
   if ((rc = tinyrad_strings_append(msgsp, err)) != TRAD_SUCCESS)
      return(errnum);

   // record call stack
   while((file = file->parent) != NULL)
   {
      snprintf(msg, sizeof(msg), "%s: %i: included dictionary file", file->path, file->line);
      if ((rc = tinyrad_strings_append(msgsp, err)) != TRAD_SUCCESS)
         return(errnum);
   };

   return(errnum);
}


/// Initialize dicitionary file buffer
///
/// @param[out] filep         pointer to buffer reference
/// @param[in]  path          path to included file
/// @param[in]  parent        parent buffer reference
/// @return returns error code
int tinyrad_file_init(
       TinyRadFile **                filep,
       const char *                  path,
       const char **                 paths,
       TinyRadFile *                 parent )
{
   size_t                  pos;
   int                     rc;
   TinyRadFile *           file;
   struct stat             sb;
   char                    fullpath[256];

   assert(filep != NULL);
   assert(path  != NULL);

   // search dictionary paths for file
   fullpath[0] = '\0';
   if ( (path[0] != '/') && ((paths)) )
   {
      for(pos = 0; ( ((paths[pos])) && (fullpath[0] == '\0') ); pos++)
      {
         snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[pos], path);
         if ((rc = tinyrad_stat(fullpath, &sb, S_IFREG)) == TRAD_SUCCESS)
            continue;
         fullpath[0] = '\0';
      };
   };

   // fall back to relative path
   if (fullpath[0] == '\0')
   {
      strncpy(fullpath, path, sizeof(fullpath));
      if ((rc = tinyrad_stat(fullpath, &sb, S_IFREG)) != TRAD_SUCCESS)
         return(rc);
   };

   // initialize buffer
   if ((file = malloc(sizeof(TinyRadFile))) == NULL)
      return(TRAD_ENOMEM);
   bzero(file, sizeof(TinyRadFile));
   file->parent = parent;

   // open dictionary for reading
   if ((file->fd = open(fullpath, O_RDONLY)) == -1)
   {
      tinyrad_file_destroy(file);
      return(TRAD_EACCES);
   };

   // store dictionary file name
   if ((file->path = strdup(fullpath)) == NULL)
   {
      tinyrad_file_destroy(file);
      return(TRAD_ENOMEM);
   };

   *filep = file;

   return(TRAD_SUCCESS);
}


/// splits line into argv and argc style elements
///
/// @param[in]  dict          dictionary reference
/// @param[in]  file          file buffer
/// @param[in]  opts          dictionary options
/// @return returns error code
int
tinyrad_file_parse_line(
         TinyRadDict *                dict,
         TinyRadFile *                file,
         uint32_t                     opts )
{
   size_t  pos;

   assert(dict != NULL);
   assert(file != NULL);
   assert(opts == 0);

   file->argc = 0;
   pos        = file->pos;

   // process line
   while ((file->buff[pos] != '\0') && (file->argc < TRAD_ARGV_SIZE))
   {
      switch(file->buff[pos])
      {
         case ' ':
         case '\t':
         break;

         case '#':
         case '\0':
         file->buff[pos] = '\0';
         return(TRAD_SUCCESS);

         default:
         file->argv[file->argc] = &file->buff[pos];
         file->argc++;
         while (file->buff[pos] != '\0')
         {
            pos++;
            switch(file->buff[pos])
            {
               case '\0':
               case '#':
               return(TRAD_SUCCESS);

               case ' ':
               case '\t':
               file->buff[pos] = '\0';
               break;

               default:
               if (file->buff[pos] < '$')
                  return(TRAD_EUNKNOWN);
               if (file->buff[pos] > 'z')
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
/// @param[in]  path          file system path
/// @param[in]  sbp           stat buffer
/// @param[in]  type          file type expecting
/// @return returns error code
int
tinyrad_stat(
         const char *                 path,
         struct stat *                sbp,
         mode_t                       type )
{
   int rc;

   assert(path != NULL);
   assert(sbp  != NULL);

   if ((rc = stat(path, sbp)) == 0)
   {
      if ((sbp->st_mode & S_IFMT) != type)
         return(TRAD_ENOENT);
      return(TRAD_SUCCESS);
   };

   switch(errno)
   {
      case EACCES:   return(TRAD_EACCES);
      case ENOENT:   return(TRAD_ENOENT);
      case ENOTDIR:  return(TRAD_ENOENT);
      default:       break;
   };

   return(TRAD_EUNKNOWN);
}


/* end of source */
