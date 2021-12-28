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
#define _LIB_LIBTINYRAD_LURL_C 1
#include "lurl.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <regex.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tinyrad_url_parser(
         const char *                  url,
         TinyRadURLDesc **             trudpp );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tinyrad_is_radius_url(
         const char *                  url )
{
   assert(url    != NULL);
   return(tinyrad_url_parser(url, NULL));
}


int
tinyrad_urldesc_alloc(
         TinyRadURLDesc **             trudpp )
{
   TinyRadURLDesc * trudp;
   assert(trudpp != NULL);
   if ((trudp = malloc(sizeof(TinyRadURLDesc))) == NULL)
      return(TRAD_ENOMEM);
   bzero(trudp, sizeof(TinyRadURLDesc));
   *trudpp = trudp;
   return(TRAD_SUCCESS);
}


char *
tinyrad_urldesc2str(
         TinyRadURLDesc *              trudp )
{
   size_t            len;
   char              buff[512];
   int               def_port;
   struct in6_addr   sin6_addr;

   assert(trudp != NULL);

   // add URL scheme
   switch(trudp->trud_opts & TRAD_SCHEME)
   {
      case TRAD_RADIUS:          strcpy(buff, "radius://");         break;
      case TRAD_RADIUS_ACCT:     strcpy(buff, "radius-acct://");    break;
      case TRAD_RADIUS_DYNAUTH:  strcpy(buff, "radius-dynauth://"); break;
      case TRAD_RADSEC:          strcpy(buff, "radsec://");         break;
      default:                   return(NULL);
   };

   // add URL host
   if (inet_pton(AF_INET6, trudp->trud_host, &sin6_addr) == 1)
   {
      strncat(buff, "[", (sizeof(buff)-strlen(buff)-1));
      strncat(buff, trudp->trud_host, (sizeof(buff)-strlen(buff)-1));
      strncat(buff, "]", (sizeof(buff)-strlen(buff)-1));
   } else {
      strncat(buff, trudp->trud_host, (sizeof(buff)-strlen(buff)-1));
   };

   // add URL port
   len = strlen(buff);
   switch(trudp->trud_opts & TRAD_SCHEME)
   {
      case TRAD_RADIUS_ACCT:     def_port = 1813; break;
      case TRAD_RADIUS_DYNAUTH:  def_port = 3799; break;
      case TRAD_RADSEC:          def_port = 2083; break;
      default:                   def_port = 1812; break;
   };
   if (trudp->trud_port != def_port)
         snprintf(&buff[len], sizeof(buff)-len-1, ":%i", trudp->trud_port);

   // add URL secret
   strncat(buff, "/", (sizeof(buff)-strlen(buff)-1));
   if ( (trudp->trud_opts & TRAD_SCHEME) != TRAD_RADSEC)
      strncat(buff, trudp->trud_secret, (sizeof(buff)-strlen(buff)-1));

   // add URL proto
   if ((trudp->trud_opts & TRAD_SCHEME) == TRAD_RADSEC)
   {
      if (!(trudp->trud_opts & TRAD_TCP))
         strncat(buff, "?udp", (sizeof(buff)-strlen(buff)-1));
   } else {
      if ((trudp->trud_opts & TRAD_TCP))
         strncat(buff, "?tcp", (sizeof(buff)-strlen(buff)-1));
   };

   return(strdup(buff));
}


void
tinyrad_urldesc_free(
         TinyRadURLDesc *              trudp )
{
   TinyRadURLDesc * next;

   while ((trudp))
   {
      next = trudp->trud_next;

      if ((trudp->trud_host))
         free(trudp->trud_host);
      if ((trudp->trud_secret))
         free(trudp->trud_secret);

      bzero(trudp, sizeof(TinyRadURLDesc));

      free(trudp);

      trudp = next;
   };

   return;
}


int
tinyrad_urldesc_parse(
         const char *                  url,
         TinyRadURLDesc **             trudpp )
{
   assert(url    != NULL);
   assert(trudpp != NULL);
   return(tinyrad_url_parser(url, trudpp));
}


int
tinyrad_url_parser(
         const char *                  url,
         TinyRadURLDesc **             trudpp )
{
   int                        rc;
   char                       buff[512];
   char *                     ptr;
   char *                     endptr;
   size_t                     pos;
   TinyRadURLDesc *           trudp;
   char *                     trud_host;
   const char *               trud_secret;
   int                        trud_port;
   int                        trud_opts;
   struct sockaddr_in6    sa6;

   assert(url    != NULL);

   trud_host     = NULL;
   trud_secret   = NULL;

   // parse URL scheme
   for(pos = 0; ( ((url[pos])) && (url[pos] != ':') ); pos++);
   if ((strncasecmp("://", &url[pos], 3)))
      return(TRAD_EURL);
   pos += 2;
   if      (!(strncasecmp(url, "radius://",         strlen("radius://"))))          trud_opts = TRAD_RADIUS;
   else if (!(strncasecmp(url, "radius-acct://",    strlen("radius-acct://"))))     trud_opts = TRAD_RADIUS_ACCT;
   else if (!(strncasecmp(url, "radius-dynauth://", strlen("radius-dynauth://"))))  trud_opts = TRAD_RADIUS_DYNAUTH;
   else if (!(strncasecmp(url, "radsec://",         strlen("radsec://"))))          trud_opts = TRAD_RADSEC;
   else return(TRAD_EURL);

   // apply defaults based on URL scheme
   switch(trud_opts)
   {
      case TRAD_RADIUS:
      trud_port         = 1812;
      break;

      case TRAD_RADIUS_ACCT:
      trud_port         = 1813;
      break;

      case TRAD_RADIUS_DYNAUTH:
      trud_port         = 3799;
      break;

      case TRAD_RADSEC:
      trud_port         = 2083;
      trud_secret       = TRAD_RADSEC_SECRET_TCP;
      trud_opts         |= TRAD_TCP;
      break;

      default:
      return(TRAD_EURL);
   };
   strncpy(buff, &url[pos+1], sizeof(buff));
   ptr = buff;

   // parse URL host
   trud_host = ptr;
   if (trud_host[0] == '[')
   {
      trud_host  = &trud_host[1];
      for(pos = 0; ( ((trud_host[pos])) && (trud_host[pos] != ']')); pos++);
      if (trud_host[pos] != ']')
         return(TRAD_EURL);
      trud_host[pos] = '\0';
      ptr       = &trud_host[pos+1];
      if (inet_pton(AF_INET6, trud_host, &sa6.sin6_addr) != 1)
         return(TRAD_EURL);
   } else {
      for(pos = 0; ( ((trud_host[pos])) && (trud_host[pos] != ':') && (trud_host[pos] != '/')); pos++)
      {
         if ( (trud_host[pos] >= 'a') && (trud_host[pos] <= 'z') )
            continue;
         if ( (trud_host[pos] >= 'A') && (trud_host[pos] <= 'Z') )
            continue;
         if ( (trud_host[pos] >= '0') && (trud_host[pos] <= '9') )
            continue;
         if ( (trud_host[pos] == '.') || (trud_host[pos] == '-') )
         {
            if ( (trud_host[pos+1] == '.') || (trud_host[pos+1] == '-') )
               return(TRAD_EURL);
            continue;
         };
         return(TRAD_EURL);
      };
      ptr = &trud_host[pos];
   };

   // parse URL port
   if (ptr[0] == ':')
   {
      ptr[0] = '\0';
      trud_port = (int)strtol(&ptr[1], &endptr, 10);
      if ( (trud_port < 1) || (trud_port > 65535) )
         return(TRAD_EURL);
      if (ptr == endptr)
         return(TRAD_EURL);
      ptr = endptr;
   };

   // parse secret
   if (ptr[0] == '/')
   {
      ptr[0] = '\0';
      for(pos = 1; ( ((ptr[pos])) && (ptr[pos] != '?') ); pos++);
      switch(trud_opts & TRAD_SCHEME)
      {
         case TRAD_RADSEC:
         if (pos != 1)
            return(TRAD_EURL);
         break;

         default:
         trud_secret = &ptr[1];
         break;
      };
      ptr = &ptr[pos];
   };

  // parse protocol
   if (ptr[0] == '?')
   {
      ptr[0] = '\0';
      if (!(strcasecmp("udp", &ptr[1])))
      {
         trud_opts &= ~(TRAD_TCP);
         if ((trud_opts & TRAD_SCHEME) == TRAD_RADSEC)
            trud_secret = TRAD_RADSEC_SECRET_UDP;
      }
      else if (!(strcasecmp("tcp", &ptr[1])))
         trud_opts |= TRAD_TCP;
      else
         return(TRAD_EURL);
   };

   if (!(trud_host[0]))
      return(TRAD_EURL);
   if (!(trud_secret))
      return(TRAD_EURL);
   if (!(trud_secret[0]))
      return(TRAD_EURL);

   if (!(trudpp))
      return(TRAD_SUCCESS);

   if ((rc = tinyrad_urldesc_alloc(&trudp)) != TRAD_SUCCESS)
      return(rc);
   trudp->trud_port       = trud_port;
   trudp->trud_opts       = trud_opts;

   if ((trudp->trud_host = strdup(trud_host)) == NULL)
   {
      tinyrad_urldesc_free(trudp);
      return(TRAD_ENOMEM);
   };

   if ((trudp->trud_secret = strdup(trud_secret)) == NULL)
   {
      tinyrad_urldesc_free(trudp);
      return(TRAD_ENOMEM);
   };

   *trudpp = trudp;

   return(TRAD_SUCCESS);
}

/* end of source */
