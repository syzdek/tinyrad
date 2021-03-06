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
#include <assert.h>

#include "lmemory.h"
#include "lerror.h"
#include "lstrings.h"


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
         TinyRadFile *                 file,
         int                           recurse )
{
   TinyRadFile * parent;

   TinyRadDebugTrace();

   while((file))
   {
      // record parent
      parent = (recurse == TRAD_FILE_RECURSE) ? file->parent : NULL;

      // free resources
      if ((file->fullpath))
         free(file->fullpath);
      if ((file->path))
         free(file->path);
      if (file->fd != -1)
         close(file->fd);
      memset(file, 0, sizeof(TinyRadFile));
      free(file);

      // pivot to parent handle
      file = parent;
   };

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
   char          msg[256];
   char **       msgs;
   size_t        max;
   size_t        pos;
   void *        ptr;

   TinyRadDebugTrace();

   // reset error
   if (!(msgsp))
      return(errnum);
   msgs   = NULL;
   *msgsp = NULL;

   // determine file path
   if (!(file))
      return(tinyrad_error_msgs(errnum, msgsp, NULL));

   // record error
   tinyrad_error_msgs(errnum, &msgs, "%s:%i: ", file->path, file->line);

   // record call stack
   while((file = file->parent) != NULL)
   {
      snprintf(msg, sizeof(msg), "in file included from %s:%i:", file->path, file->line);
      tinyrad_strsadd(&msgs, msg);
   };

   // invert errors
   max = tinyrad_strscount(msgs);
   for(pos = 0; (pos < (max >> 1)); pos++)
   {
      ptr             = msgs[pos];
      msgs[pos]       = msgs[max-pos-1];
      msgs[max-pos-1] = ptr;
   };

   *msgsp = msgs;

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
       char **                       paths,
       TinyRadFile *                 parent )
{
   size_t                  pos;
   int                     rc;
   TinyRadFile *           file;
   struct stat             sb;
   char                    fullpath[256];

   TinyRadDebugTrace();

   assert(filep != NULL);
   assert(path  != NULL);

   *filep = NULL;

   // initialize buffer
   if ((file = malloc(sizeof(TinyRadFile))) == NULL)
      return(TRAD_ENOMEM);
   memset(file, 0, sizeof(TinyRadFile));

   // store dictionary file name
   if ((file->path = tinyrad_strdup(path)) == NULL)
   {
      tinyrad_file_destroy(file, TRAD_FILE_NORECURSE);
      return(TRAD_ENOMEM);
   };

   file->parent = parent;
   *filep = file;

   // search dictionary paths for file
   fullpath[0] = '\0';
   if ( (path[0] != '/') && ((paths)) )
   {
      for(pos = 0; ( ((paths[pos])) && (fullpath[0] == '\0') ); pos++)
      {
         snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[pos], path);
         if (tinyrad_stat(fullpath, &sb, S_IFREG) == TRAD_SUCCESS)
            continue;
         fullpath[0] = '\0';
      };
   };

   // fall back to relative path
   if (fullpath[0] == '\0')
   {
      tinyrad_strlcpy(fullpath, path, sizeof(fullpath));
      if ((rc = tinyrad_stat(fullpath, &sb, S_IFREG)) != TRAD_SUCCESS)
         return(rc);
   };

   // store dictionary file name
   if ((file->fullpath = tinyrad_strdup(fullpath)) == NULL)
      return(TRAD_ENOMEM);

   // open dictionary for reading
   if ((file->fd = open(fullpath, O_RDONLY)) == -1)
      return(TRAD_EACCES);

   return(TRAD_SUCCESS);
}


int
tinyrad_filetostr(
         char *                        dst,
         const char *                  filename,
         size_t                        size )
{
   int            rc;
   int            fd;
   struct stat    sb;
   size_t         len;
   ssize_t        rlen;

   assert(dst  != NULL);
   assert(size  > 1);

   if ((rc = tinyrad_stat(filename, &sb, S_IFREG)) != TRAD_SUCCESS)
      return((int)rc);

   len = (size > (size_t)(sb.st_size+1)) ? (size_t)sb.st_size : (size - 1);

   if ((fd = open(filename, O_RDONLY)) == -1)
      return(TRAD_EACCES);

   if ((rlen = read(fd, dst, len)) == -1)
   {
      close(fd);
      return(TRAD_EUNKNOWN);
   };
   dst[rlen] = '\0';

   close(fd);

   return(TRAD_SUCCESS);
}


int
tinyrad_readline(
         int                           fd,
         char *                        str,
         size_t                        size,
         size_t *                      bytes_read )
{
   ssize_t           rc;
   struct stat       sb;
   size_t            pos;

   // check file descriptor
   if (fd == -1)
      return(TRAD_EINVAL);
   if (fstat(fd, &sb) == -1)
      return(TRAD_EINVAL);

   if ((bytes_read))
      *bytes_read = 0;

   // process input (slow, but works on TCP sockets, PIPE, FIFO, and  regular files)
   for(pos = 0; (pos < (size-1)); pos++)
   {
      // read data
      if ((rc = read(fd, &str[pos], 1)) == -1)
         return(TRAD_EUNKNOWN);

      // process end of file
      if (rc == 0)
      {
         str[pos] = '\0';
         if ((bytes_read))
            *bytes_read = pos;
         return(TRAD_SUCCESS);
      };

      // continue if not end of file
      if (str[pos] != '\n')
         continue;

      // process end of line
      str[pos] = '\0';
      if ((pos > 0) && (str[pos-1] == '\r'))
         str[pos-1] = '\0';
      if ((bytes_read))
         *bytes_read = (pos+1);
      return(TRAD_SUCCESS);
   };

   str[pos] = '\0';
   if ((bytes_read))
      *bytes_read = (pos+1);

   return(TRAD_ENOBUFS);
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
   TinyRadDebugTrace();

   assert(path != NULL);
   assert(sbp  != NULL);

   if (stat(path, sbp) == 0)
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
