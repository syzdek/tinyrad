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
#define _LIB_LIBTINYRAD_LSTRINGS_C 1
#include "lstrings.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <assert.h>

#include "ldict.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


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

//------------------//
// string functions //
//------------------//
#pragma mark string functions

char *
tinyrad_strdup(
         const char *                  s1 )
{
   char *      ptr;
   size_t      len;
   if (!(s1))
      return(NULL);
   len = strlen(s1);
   if ((ptr = malloc(len+2)) == NULL)
      return(NULL);
   memcpy(ptr, s1, len);
   ptr[len+0] = '\0';
   ptr[len+1] = '\0';
   return(ptr);
}


size_t
tinyrad_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t len;
   assert(dst     != NULL);
   assert(src     != NULL);
   assert(dstsize  > 0);
   for(len = 0; ((src[len])); len++)
      if (len < dstsize)
         dst[len] = src[len];
   dst[((len < dstsize) ? len : (dstsize-1))] = '\0';
   return(len);
}


size_t
tinyrad_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      pos;
   size_t      offset;
   size_t      len;
   assert(dst     != NULL);
   assert(src     != NULL);
   assert(dstsize  > 0);
   for(pos = 0; ((dst[pos])); pos++);
   for(offset = 0; ((src[offset])); offset++)
      if ((pos + offset) < dstsize)
         dst[pos+offset] = src[offset];
   len = offset + pos;
   dst[((len < dstsize) ? len : (dstsize-1))] = '\0';
   return(len);
}


char *
tinyrad_strndup(
         const char *                  s1,
         size_t                        n )
{
   char *      ptr;
   size_t      len;
   if (!(s1))
      return(NULL);
   len = strlen(s1);
   len = (len < n) ? len : n;
   if ((ptr = malloc(len+2)) == NULL)
      return(NULL);
   memcpy(ptr, s1, len);
   ptr[len+0] = '\0';
   ptr[len+1] = '\0';
   return(ptr);
}


/// Appends string to NULL terminated array of strings
///
/// @param[out] strsp         pointer to string array
/// @param[in]  str           string to append to array
/// @return returns error code
int
tinyrad_strsadd(
         char ***                      strsp,
         const char *                  str )
{
   size_t     count;
   char **    strs;

   TinyRadDebugTrace();

   assert(strsp != NULL);
   assert(str   != NULL);

   count = tinyrad_strscount(*strsp);

   // increase size of array
   if ((strs = realloc(*strsp, (sizeof(char *)*(count+2)))) == NULL)
      return(TRAD_ENOMEM);
   *strsp        = strs;

   // copy string
   if ((strs[count] = tinyrad_strdup(str)) == NULL)
      return(TRAD_ENOMEM);

   // terminate array
   count++;
   strs[count] = NULL;

   return(TRAD_SUCCESS);
}


/// counts number of strings in NULL terminated array of strings
///
/// @param[in]  strs          pointer to string array
/// @return number of strings in array
size_t
tinyrad_strscount(
         char * const *                strs )
{
   size_t count;
   TinyRadDebugTrace();
   if (!(strs))
      return(0);
   for(count = 0; ((strs != NULL)&&(strs[count] != NULL)); count++);
   return(count);
}


int
tinyrad_strsdup(
         char ***                      dstp,
         char * const *                src )
{
   size_t      len;
   size_t      size;
   size_t      pos;
   char **     dst;

   TinyRadDebugTrace();

   assert(dstp != NULL);
   assert(src   != NULL);

   len  = tinyrad_strscount(src);
   size = sizeof(char *) * (len+1);

   if ((dst = malloc(size)) == NULL)
      return(TRAD_ENOMEM);

   for(pos = 0; (pos < len); pos++)
   {
      if ((dst[pos] = tinyrad_strdup(src[pos])) == NULL)
      {
         tinyrad_strsfree(dst);
         return(TRAD_ENOMEM);
      };
   };
   dst[pos] = NULL;

   *dstp = dst;

   return(TRAD_SUCCESS);
}


/// frees NULL terminated array of strings
///
/// @param[in]  strs          pointer to string array
void
tinyrad_strsfree(
         char **                       strs )
{
   int i;
   TinyRadDebugTrace();
   if (!(strs))
      return;
   for(i = 0; ((strs[i])); i++)
   {
      free(strs[i]);
      strs[i] = NULL;
   };
   free(strs);
   return;
}


int
tinyrad_strtoargs(
         char *                        str,
         char ***                      argvp,
         int *                         argcp )
{
   char           quote;
   char *         bol;
   char **        argv;
   size_t         pos;
   int            rc;

   assert(str != NULL);

   argv = NULL;

   for(pos = 0; ((str[pos])); pos++)
   {
      switch(str[pos])
      {
         // skip white space
         case ' ':
         case '\t':
         break;

         // parse quoted argument
         case '"':
         case '\'':
         quote = str[pos];
         bol   = &str[pos+1];
         for( pos += 1; ((bol[pos] != '\0') && (bol[pos] != quote)); pos++);
         if (bol[pos] != quote)
         {
            tinyrad_strsfree(argv);
            return(TRAD_ESYNTAX);
         };
         bol[pos] = '\0';
         pos++;
         if ( (bol[pos] != ' ') && (bol[pos] != '\t') && (bol[pos] != '\0') )
         {
            tinyrad_strsfree(argv);
            return(TRAD_ESYNTAX);
         };
         if ((rc = tinyrad_strsadd(&argv, bol)) != TRAD_SUCCESS)
         {
            tinyrad_strsfree(argv);
            return(rc);
         };
         break;

         // parse unquoted argument
         default:
         bol = &str[pos];
         for(pos = 0; ((bol[pos] != ' ')&&(bol[pos] != '\t')&&(bol[pos] != '\0')); pos++)
         {
            if ( (!(isalnum(bol[pos]))) && (bol[pos] != '.') && (bol[pos] != '-') && (bol[pos] != '_') )
            {
               tinyrad_strsfree(argv);
               return(TRAD_ESYNTAX);
            };
         };
         bol[pos] = '\0';
         if ((rc = tinyrad_strsadd(&argv, bol)) != TRAD_SUCCESS)
         {
            tinyrad_strsfree(argv);
            return(rc);
         };
         break;
      };
   };

   if ((argcp))
      *argcp = (int)tinyrad_strscount(argv);
   if ((argvp))
      *argvp = argv;
   if (!(argvp))
      tinyrad_strsfree(argv);

   return(TRAD_SUCCESS);
}



/* end of source */
