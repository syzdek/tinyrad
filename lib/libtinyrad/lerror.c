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
#define _LIB_LIBTINYRAD_LERROR_C 1
#include "lerror.h"


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
#include <stdarg.h>

#include "lmemory.h"

//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

const char *
tinyrad_error_map(
         int                           errnum );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

const char *
tinyrad_error_map(
         int                           errnum )
{
   switch(errnum)
   {
      case TRAD_SUCCESS:    return("success");
      case TRAD_ENOMEM:     return("out of virtual memory");
      case TRAD_EACCES:     return("permission denied");
      case TRAD_ENOBUFS:    return("no buffer space available");
      case TRAD_ENOENT:     return("no such file or directory");
      case TRAD_ESYNTAX:    return("invalid or unrecognized syntax");
      case TRAD_EEXISTS:    return("dictionary object exists");
      default:
      break;
   };
   return("unknown error code");
}


int
tinyrad_error_msgs(
         int                           errnum,
         char ***                      msgsp,
         const char *                  fmt,
         ... )
{
   const char *   errmsg;
   char           msg[256];
   va_list        args;

   if (!(msgsp))
      return(errnum);

   msg[0] = '\0';

   if ((fmt))
   {
      va_start(args, fmt);
      vsnprintf(msg, sizeof(msg), fmt, args);
      va_end(args);
   };

   errmsg = tinyrad_strerror(errnum);
   strcat(msg, errmsg);

   tinyrad_strings_append(msgsp, errmsg);

   return(errnum);
}


/// Return error string of error code
///
/// @param[in]  errnum        error number
/// @return returns error string
const char *
tinyrad_strerror(
         int                           errnum )
{
   const char * msg;
   if ((msg = tinyrad_error_map(errnum)) == NULL)
      msg = "unknown error code";
   return(msg);
}


/// Copy error string into buffer
///
/// @param[in]  errnum        error number
/// @param[out] strerrbuf     string buffer
/// @param[in]  buflen        size of string buffer
/// @return returns error code
int
tinyrad_strerror_r(
         int                           errnum,
         char *                        strerrbuf,
         size_t                        buflen )
{
   const char * msg;

   assert(strerrbuf != NULL);
   assert(buflen    != 0);

   if ((msg = tinyrad_error_map(errnum)) == NULL)
   {
      snprintf(strerrbuf, buflen, "unknown error code %i", errnum);
      return(TRAD_SUCCESS);
   };

   strncpy(strerrbuf, msg, buflen);
   strerrbuf[buflen-1] = '\0';

   return(TRAD_SUCCESS);
}

/* end of source */
