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


/// splits line into argv and argc style elements
///
/// @param[in]  dict          dictionary reference
/// @param[in]  buff          file buffer
/// @param[in]  opts          dictionary options
/// @return returns error code
int
tinyrad_file_parse_line(
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
