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
#include <netdb.h>

#include <stdio.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tinyrad_urldesc_parser(
         char *                        url,
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
   char                       buff[512];
   TinyRadDebugTrace();
   assert(url    != NULL);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   ====> %s(\"%s\")", __func__, url);
   strncpy(buff, url, sizeof(buff));
   return(tinyrad_urldesc_parser(buff, NULL));
}


int
tinyrad_urldesc_alloc(
         TinyRadURLDesc **             trudpp )
{
   TinyRadURLDesc * trudp;
   TinyRadDebugTrace();
   assert(trudpp != NULL);
   if ((trudp = malloc(sizeof(TinyRadURLDesc))) == NULL)
      return(TRAD_ENOMEM);
   memset(trudp, 0, sizeof(TinyRadURLDesc));
   *trudpp = trudp;
   return(TRAD_SUCCESS);
}


char *
tinyrad_urldesc2str(
         TinyRadURLDesc *              trudp )
{
   size_t            len;
   char              buff[512];
   size_t            x;
   size_t            y;
   int               def_port;
   char              hex[3];
   struct in6_addr   sin6_addr;

   TinyRadDebugTrace();

   assert(trudp != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s(trudp)", __func__);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_host:   %s", trudp->trud_host);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_port:   %i", trudp->trud_port);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_secret: %s", trudp->trud_secret);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_opts:   0x%04x", trudp->trud_opts);

   memset(buff, 0, sizeof(buff));

   while ((trudp))
   {
      // add URL scheme
      len = (sizeof(buff)-strlen(buff)-1);
      switch(trudp->trud_opts & TRAD_SCHEME)
      {
         case TRAD_RADIUS:          strncat(buff, "radius://",         len); break;
         case TRAD_RADIUS_ACCT:     strncat(buff, "radius-acct://",    len); break;
         case TRAD_RADIUS_DYNAUTH:  strncat(buff, "radius-dynauth://", len); break;
         case TRAD_RADSEC:          strncat(buff, "radsec://",         len); break;
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
      {
         len = strlen(buff);
         strncat(buff, trudp->trud_secret, (sizeof(buff)-strlen(buff)-1));
         for(x = len; ( (x < sizeof(buff)) && ((buff[x])) ); x++)
         {
            if ((buff[x] >= 'a') && (buff[x] <= 'z'))
               continue;
            if ((buff[x] >= 'A') && (buff[x] <= 'Z'))
               continue;
            if ((buff[x] >= '0') && (buff[x] <= '9'))
               continue;
            if ( (buff[x] == '-') || (buff[x] == '_') || (buff[x] == '.') || (buff[x] == '~') )
               continue;
            if (buff[x] == ' ')
            {
               buff[x] = '+';
               continue;
            };
            for(y = x; ((buff[y])); y++);
            for(; (y > x); y--)
               buff[y+2] = buff[y];
            snprintf(hex, sizeof(hex), "%02x", buff[x]);
            buff[x+0] = '%';
            buff[x+1] = hex[0];
            buff[x+2] = hex[1];
            x += 2;
         };
      };

      // add URL proto
      if ((trudp->trud_opts & TRAD_SCHEME) == TRAD_RADSEC)
      {
         if (!(trudp->trud_opts & TRAD_TCP))
            strncat(buff, "?udp", (sizeof(buff)-strlen(buff)-1));
      } else {
         if ((trudp->trud_opts & TRAD_TCP))
            strncat(buff, "?tcp", (sizeof(buff)-strlen(buff)-1));
      };

      if ((trudp->trud_next))
         strncat(buff, " ", (sizeof(buff)-strlen(buff)-1));

      trudp = trudp->trud_next;
   };

   return(strdup(buff));
}


void
tinyrad_urldesc_free(
         TinyRadURLDesc *              trudp )
{
   TinyRadURLDesc * next;

   TinyRadDebugTrace();

   while ((trudp))
   {
      next = trudp->trud_next;

      if ((trudp->trud_host))
         free(trudp->trud_host);
      if ((trudp->trud_secret))
         free(trudp->trud_secret);

      memset(trudp, 0, sizeof(TinyRadURLDesc));

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
   int                        rc;
   char                       buff[512];
   char *                     str;
   char *                     ptr;
   TinyRadURLDesc **          pptr;

   TinyRadDebugTrace();

   assert(url    != NULL);
   assert(trudpp != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   ====> %s(\"%s\")", __func__, url);

   strncpy(buff, url, sizeof(buff));

   pptr = trudpp;
   str  = buff;

   while((ptr = index(str, ' ')) != NULL)
   {
      ptr[0] = '\0';
      if (!(str[0]))
      {
         str = &ptr[1];
         continue;
      };
      if ((rc = tinyrad_urldesc_parser(str, pptr)) != TRAD_SUCCESS)
         return(rc);
      pptr = &(*pptr)->trud_next;
      str = &ptr[1];
   };

   if ((str[0]))
   {
      if ((rc = tinyrad_urldesc_parser(str, pptr)) != TRAD_SUCCESS)
         return(rc);
      pptr = &(*pptr)->trud_next;
   };

   if (pptr == trudpp)
      return(TRAD_EURL);

   if (!(tinyrad_urldesc_scheme(*trudpp)))
      return(TRAD_EURL);

   return(TRAD_SUCCESS);
}


int
tinyrad_urldesc_parser(
         char *                        url,
         TinyRadURLDesc **             trudpp )
{
   int                        rc;
   char *                     ptr;
   char *                     endptr;
   size_t                     pos;
   size_t                     x;
   TinyRadURLDesc *           trudp;
   char *                     trud_host;
   const char *               trud_secret;
   int                        trud_port;
   unsigned                   trud_opts;
   char                       hex[3];
   struct sockaddr_in6    sa6;

   TinyRadDebugTrace();

   assert(url    != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   ====> %s(\"%s\")", __func__, url);

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
   ptr = &url[pos+1];

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
      for(pos = 1; ( ((ptr[pos])) && (ptr[pos] != '?') ); pos++)
      {
         if ( (ptr[pos] >= '0') && (ptr[pos] <= '9'))
            continue;
         if ( (ptr[pos] >= 'a') && (ptr[pos] <= 'z'))
            continue;
         if ( (ptr[pos] >= 'A') && (ptr[pos] <= 'Z'))
            continue;
         switch(ptr[pos])
         {
            case '+':
            case '-':
            case '_':
            case '.':
            case '~':
            break;

            case '%':
            if ( ((ptr[pos+1] < 'a') || (ptr[pos+1] > 'f')) &&
                 ((ptr[pos+1] < 'A') || (ptr[pos+1] > 'F')) &&
                 ((ptr[pos+1] < '0') || (ptr[pos+1] > '9')) )
               return(TRAD_EURL);
            if ( ((ptr[pos+2] < 'a') || (ptr[pos+2] > 'f')) &&
                 ((ptr[pos+2] < 'A') || (ptr[pos+2] > 'F')) &&
                 ((ptr[pos+2] < '0') || (ptr[pos+2] > '9')) )
               return(TRAD_EURL);
            break;

            default:
            return(TRAD_EURL);
         };
      };
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
      ptr = &ptr[4];
   };

   if (ptr[0] != '\0')
      return(TRAD_EURL);
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
   for(pos = 0; ((trudp->trud_secret[pos])); pos++)
   {
      switch(trudp->trud_secret[pos])
      {
         case '+':
         trudp->trud_secret[pos] = ' ';
         break;

         case '%':
         hex[0] = trudp->trud_secret[pos+1];
         hex[1] = trudp->trud_secret[pos+2];
         hex[2] = '\0';
         trudp->trud_secret[pos] = (char)strtol(hex, NULL, 16);
         for(x = pos+1; ((trudp->trud_secret[x+2])); x++)
            trudp->trud_secret[x] = trudp->trud_secret[x+2];
         trudp->trud_secret[x] = trudp->trud_secret[x+2];
         break;

         default:
         break;
      };
   };

   *trudpp = trudp;

   return(TRAD_SUCCESS);
}


int
tinyrad_urldesc_resolve(
         TinyRadURLDesc *              trudp,
         uint32_t                      opts )
{
   TinyRadURLDesc *              trudp_ptr;
   size_t                        pos;
   int                           ai_family;
   int                           ai_flags;
   size_t                        size;
   struct addrinfo               hints;
   struct addrinfo *             hintsp;
   struct addrinfo *             res;
   struct addrinfo *             next;
   void *                        ptr;
   size_t                        sas_len;
   struct sockaddr_storage **    sasp;

   TinyRadDebugTrace();

   assert(trudp            != NULL);
   assert(trudp->trud_host != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s(trudp)", __func__);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_host:   %s", trudp->trud_host);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_port:   %i", trudp->trud_port);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_secret: %s", trudp->trud_secret);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   => trudp->trud_opts:   0x%04x", trudp->trud_opts);

   // clear existing sockaddrs
   for(trudp_ptr = trudp; ((trudp_ptr)); trudp_ptr = trudp_ptr->trud_next)
   {
      if ((trudp_ptr->sockaddrs))
      {
         for(pos = 0; (pos < trudp_ptr->sockaddrs_len); pos++)
            free(trudp_ptr->sockaddrs[pos]);
         free(trudp_ptr->sockaddrs);
      };
      trudp_ptr->sockaddrs       = NULL;
      trudp_ptr->sockaddrs_len   = 0;
   };

   ai_flags  = ((opts & TRAD_SERVER)) ? (AI_NUMERICHOST | AI_NUMERICSERV) : 0;
   ai_flags |= AI_ADDRCONFIG;
   switch(opts & TRAD_IP_UNSPEC)
   {
      case 0:
      ai_family  = PF_UNSPEC;
      break;

      case TRAD_IPV4:
      ai_family = PF_INET;
      break;

      case TRAD_IPV6:
      ai_family = PF_INET6;
      break;

      default:
      ai_family  = PF_INET6;
      ai_flags  |= AI_V4MAPPED | AI_ALL;
      break;
   };

   while ((trudp))
   {
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_flags    = ai_flags;
      hints.ai_family   = ai_family;
      hints.ai_socktype = ((opts & TRAD_TCP) == 0) ? SOCK_DGRAM  : SOCK_STREAM;
      hints.ai_protocol = ((opts & TRAD_TCP) == 0) ? IPPROTO_UDP : IPPROTO_TCP;
      hintsp            = &hints;

      if (getaddrinfo(trudp->trud_host, NULL, hintsp, &res) != 0)
         return(TRAD_ERESOLVE);

      // initialize memory
      sas_len = ((trudp->sockaddrs_len)) ? trudp->sockaddrs_len : 0;
      if ((sasp = trudp->sockaddrs) == NULL)
      {
         if ((sasp = malloc(sizeof(struct sockaddr_storage *))) == NULL)
            return(TRAD_ENOMEM);
         sasp[0]              = NULL;
         trudp->sockaddrs     = sasp;
         sas_len              = 0;
         trudp->sockaddrs_len = sas_len;
      };

      next = res;
      while((next))
      {
         size = sizeof(struct sockaddr_storage *) * (sas_len+2);
         if ((ptr = realloc(sasp, size)) == NULL)
            return(TRAD_ENOMEM);
         sasp              = ptr;
         trudp->sockaddrs  = ptr;
         sasp[sas_len+1]   = NULL;

         if ((sasp[sas_len] = malloc(sizeof(struct sockaddr_storage))) == NULL)
            return(TRAD_ENOMEM);
         memset(sasp[sas_len], 0, sizeof(struct sockaddr_storage));
         memcpy(sasp[sas_len], next->ai_addr, next->ai_addrlen);

         switch(sasp[sas_len]->ss_family)
         {
            case AF_INET:
            ((struct sockaddr_in *)sasp[sas_len])->sin_port = htons((uint16_t)trudp->trud_port);
            break;

            case AF_INET6:
            ((struct sockaddr_in6 *)sasp[sas_len])->sin6_port = htons((uint16_t)trudp->trud_port);
            break;

            default:
            break;
         };

         sas_len++;
         trudp->sockaddrs_len = sas_len;

         next = next->ai_next;
      };

      freeaddrinfo(res);

      trudp = trudp->trud_next;
   };

   return(TRAD_SUCCESS);
}


unsigned
tinyrad_urldesc_scheme(
         TinyRadURLDesc *             trudp )
{
   unsigned             scheme;
   TinyRadURLDesc *     ptr;

   assert(trudp != NULL);

   scheme = 0;

   for(ptr = trudp; ((ptr)); ptr = ptr->trud_next)
   {
      if (!(ptr->trud_opts & TRAD_SCHEME))
         return(0);
      if ((ptr->trud_opts & TRAD_SCHEME) == TRAD_RADSEC)
         continue;
      if (!(scheme))
         scheme = ptr->trud_opts & TRAD_SCHEME;
      if ((ptr->trud_opts & TRAD_SCHEME) != scheme)
         return(0);
   };

   switch(scheme)
   {
      case 0:
      return(TRAD_RADSEC);

      case TRAD_RADIUS:
      case TRAD_RADIUS_ACCT:
      case TRAD_RADIUS_DYNAUTH:
      return(scheme);

      default:
      break;
   };

   return(0);
}


/* end of source */
