/*
 *  Tiny RADIUS Client Library
 *  Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.
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
#define _LIB_LIBTRUTILS_LFILE_C 1
#include "lfile.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>


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

void
trutils_close(
         TRUtilsFile *                 fb )
{
   if (!(fb))
      return;

   if (fb->fd != -1)
      close(fb->fd);

   if ((fb->filename))
      free(fb->filename);

   free(fb);

   return;
}


TRUtilsFile *
trutils_open(
         unsigned                      opts,
         const char *                  filename )
{
   TRUtilsFile *  fb;
   struct stat    sb;

   assert(filename != NULL);

   // verify file exists
   if (stat(filename, &sb) == -1)
   {
      trutils_error(opts,  NULL, "%s: %s", filename, strerror(errno));
      return(NULL);
   };
   if ((sb.st_mode & S_IFMT) != S_IFREG)
   {
      trutils_error(opts,  NULL, "%s: is not a file", filename);
      return(NULL);
   };

   // allocate initial
   if ((fb = malloc(sizeof(TRUtilsFile))) == NULL)
   {
      trutils_error(opts,  NULL, "out of virtual memory");
      return(NULL);
   };
   memset(fb, 0, sizeof(TRUtilsFile));
   fb->fd = -1;

   // copy filename
   if ((fb->filename = strdup(filename)) == NULL)
   {
      trutils_error(opts,  NULL, "out of virtual memory");
      trutils_close(fb);
      return(NULL);
   };

   // open file
   if ((fb->fd = open(filename, O_RDONLY)) == -1)
   {
      trutils_error(opts,  NULL, "%s: %s", filename, strerror(errno));
      trutils_close(fb);
      return(NULL);
   };

   return(fb);
}


int
trutils_readline(
         unsigned                      opts,
         TRUtilsFile *                 fb )
{
   size_t      offset;
   size_t      pos;
   size_t      size;
   ssize_t     rc;

   assert(fb != 0);

   // adjusts beginning of line
   fb->bol = ((fb->eol)) ? &fb->eol[1] : fb->buff;

   // look for end line
   if ((fb->eol = strchr(fb->bol, '\n')) != NULL)
   {
      fb->eol[0] = '\0';
      return(1);
   };

   // shift buffer
   offset = (size_t)(fb->bol = fb->buff);
   for(pos = 0; ((pos+offset) < fb->len); pos++)
      fb->buff[pos] = fb->buff[pos+offset];
   fb->buff[pos]  = '\0';
   fb->len       -= offset;

   // attempt to fill buffer
   size = sizeof(fb->buff) - fb->len - 1;
   if ((rc = read(fb->fd, &fb->buff[fb->len], size)) == -1)
   {
      trutils_error(opts, NULL, "%s: %s", fb->filename, strerror(errno));
      return(-1);
   };
   fb->len           += (size_t)rc;
   fb->buff[fb->len]  = '\0';
   fb->bol            = fb->buff;
   if (!(fb->len))
      return(0);

   // look for end line
   if ((fb->eol = strchr(fb->bol, '\n')) != NULL)
   {
      fb->eol[0] = '\0';
      return(1);
   };

   fb->eol = &fb->bol[strlen(fb->bol)];

   return(0);
}


/* end of source */
