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
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
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


int
tinyrad_strexpand(
         char *                        dst,
         const char * restrict         src,
         size_t                        len )
{
   size_t            pos;
   size_t            offset;
   char              buff[4096];
   char *            ptr;
   struct utsname    unam;
   struct passwd     pwd;
   struct passwd *   pwres;
   struct group      grp;
   struct group *    grres;


   assert(dst != NULL);
   assert(src != NULL);
   assert(len  > 0);

   // expand escapes in buffer
   offset = 0;
   for(pos = 0; ( ((src[pos])) && (offset < (len-1)) ); pos++)
   {
      switch(src[pos])
      {
         // tokens
         case '%':
         // %% - a literal percent
         // %D - domain name
         // %d - home directory
         // %G - group ID
         // %g - group name
         // %H - hostname with domain name
         // %h - hostname without domain name
         // %P - process ID
         // %p - process name
         // %U - user ID
         // %u - username
         pos++;
         switch(src[pos])
         {
            case 'D': // domain name
            if (uname(&unam) == -1)
               return(TRAD_EUNKNOWN);
            strncpy(buff, unam.nodename, sizeof(buff));
            if ((ptr = strchr(buff, '.')) == NULL)
               buff[0] = '\0';
            ptr = ((ptr)) ? &ptr[1] : buff;
            dst[offset] = '\0';
            tinyrad_strlcat(dst, ptr, len);
            offset += strlen(ptr);
            break;

            case 'd': // home directory
            getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &pwres);
            if (!(pwres))
               break;
            dst[offset] = '\0';
            tinyrad_strlcat(dst, pwres->pw_dir, len);
            offset += strlen(pwres->pw_dir);
            break;

            case 'G': // gid
            snprintf(buff, sizeof(buff), "%u", getgid());
            dst[offset] = '\0';
            tinyrad_strlcat(dst, buff, len);
            offset += strlen(buff);
            break;

            case 'g': // group name
            getgrgid_r(getgid(), &grp, buff, sizeof(buff), &grres);
            if (!(grres))
               break;
            dst[offset] = '\0';
            tinyrad_strlcat(dst, grres->gr_name, len);
            offset += strlen(grres->gr_name);
            break;

            case 'H': // fully qualified hostname
            if (uname(&unam) == -1)
               break;
            dst[offset] = '\0';
            tinyrad_strlcat(dst, unam.nodename, len);
            offset += strlen(unam.nodename);
            break;

            case 'h': // short hostname
            if (uname(&unam) == -1)
               break;
            strncpy(buff, unam.nodename, sizeof(buff));
            if ((ptr = strchr(buff, '.')) != NULL)
               ptr[0] = '\0';
            dst[offset] = '\0';
            tinyrad_strlcat(dst, buff, len);
            offset += strlen(buff);
            break;

            case 'P': // process ID
            snprintf(buff, sizeof(buff), "%u", getppid());
            dst[offset] = '\0';
            tinyrad_strlcat(dst, buff, len);
            offset += strlen(buff);
            break;

            case 'p': // process name/ident
            dst[offset] = '\0';
            tinyrad_strlcat(dst, tinyrad_debug_ident, len);
            offset += strlen(tinyrad_debug_ident);
            break;

            case 'U': // uid
            snprintf(buff, sizeof(buff), "%u", getgid());
            dst[offset] = '\0';
            tinyrad_strlcat(dst, buff, len);
            offset += strlen(buff);
            break;

            case 'u': // username
            getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &pwres);
            if (!(pwres))
               break;
            dst[offset] = '\0';
            tinyrad_strlcat(dst, pwres->pw_name, len);
            offset += strlen(pwres->pw_name);
            break;

            case '%': // '%' character
            dst[offset++] = src[pos];
            break;

            default:
            dst[offset++] = src[pos];
         };
         break;

         // character escape sequences
         case '\\':
         pos++;
         switch(src[pos])
         {
            case '"':
            case '\'':
            case '\\':
            case '\?':
            dst[offset++] = src[pos];
            break;

            case 'n': dst[offset++] = '\n'; break;
            case 'r': dst[offset++] = '\r'; break;
            case 't': dst[offset++] = '\t'; break;
            case 'v': dst[offset++] = '\v'; break;

            // \xnn - treat nn as hex value
            case 'x':
            buff[0] = src[++pos];
            buff[1] = src[++pos];
            buff[2] = '\0';
            dst[offset++] = (char)strtoul(buff, &ptr, 16);
            if (ptr != &buff[2])
               return(TRAD_SUCCESS);
            break;

            // \nnn - treat nnn as octal value
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            buff[0] = src[pos++];
            buff[1] = src[pos++];
            buff[2] = src[pos];
            buff[3] = '\0';
            dst[offset++] = (char)strtoul(buff, &ptr, 8);
            if (ptr != &buff[3])
               return(TRAD_SUCCESS);
            break;

            default:
            dst[offset++] = src[pos];
         };
         break;

         default:
         dst[offset++] = src[pos];
         break;
      };
   };
   if (offset >= len)
   {
      dst[len-1] = '\0';
      return(TRAD_ENOBUFS);
   };
   dst[offset] = '\0';

   return(TRAD_SUCCESS);
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
tinyrad_strsplit(
         const char *                  str,
         int                           delim,
         char ***                      argvp,
         int *                         argcp )
{
   int            rc;
   size_t         pos;
   char **        argv;
   char *         line;
   char *         bol;

   assert(str != NULL);

   if (!(str))
   {
      if ((argvp))
         *argvp = NULL;
      if ((argcp))
         *argcp = 0;
      return(TRAD_SUCCESS);
   };

   if ((line = tinyrad_strdup(str)) == NULL)
      return(TRAD_ENOMEM);

   argv = NULL;
   bol  = line;

   for(pos = 0; ((line[pos])); pos++)
   {
      if (line[pos] == delim)
      {
         line[pos] = '\0';
         if ((rc = tinyrad_strsadd(&argv, bol)) != TRAD_SUCCESS)
         {
            free(line);
            tinyrad_strsfree(argv);
            return(rc);
         };
         bol = &line[pos];
      };
   };

   if ((rc = tinyrad_strsadd(&argv, bol)) != TRAD_SUCCESS)
   {
      free(line);
      tinyrad_strsfree(argv);
      return(rc);
   };

   return(TRAD_SUCCESS);
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
   int            eol;

   assert(str != NULL);

   argv = NULL;
   eol  = 0;

   for(pos = 0; ( ((str[pos])) && (!(eol)) ); pos++)
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
         bol   = &str[pos];
         for(pos += 1; ((str[pos] != '\0') && (str[pos] != quote)); pos++)
            if ( (quote == '"') && (str[pos] == '\\') && (str[pos+1] == '"') )
               pos++;
         if (str[pos] != quote)
         {
            tinyrad_strsfree(argv);
            return(TRAD_ESYNTAX);
         };
         pos++;
         switch(str[pos])
         {
            case '\0':
            case '#':
            eol = 1;
            str[pos] = '\0';
            break;

            case ' ':
            case '\t':
            str[pos] = '\0';
            break;

            default:
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
         while( (str[pos] != ' ') && (str[pos] != '\t') && (str[pos] != '\0') && (str[pos] != '#') )
         {
            if ( (str[pos] < '!') || (str[pos] > '~') || (str[pos] == '"') || (str[pos] == '\'') )
            {
               tinyrad_strsfree(argv);
               return(TRAD_ESYNTAX);
            };
            pos++;
         };
         if ((str[pos] == '#') || (str[pos] == '\0'))
            eol = 1;
         str[pos] = '\0';
         if ((bol[0]))
         {
            if ((rc = tinyrad_strsadd(&argv, bol)) != TRAD_SUCCESS)
            {
               tinyrad_strsfree(argv);
               return(rc);
            };
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


char *
tinyrad_strtrim(
         char *                        str )
{
   ssize_t offset;
   ssize_t pos;

   if (!(str))
      return(str);

   // find end of whitespace
   for(offset = 0; ( ((str[offset])) && ((isspace(str[offset]))) ); offset++);

   // shift string by offset
   for(pos = offset; ((str[pos])); pos++)
      str[pos-offset] = str[pos];
   if (!(pos))
      return(str);

   // trim end of line
   for(pos = pos - 1 - offset; ( ((isspace(str[pos]))) && (pos > 0) ); pos--);
   str[pos+1] = '\0';

   return(str);
}



/* end of source */
