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
#define _LIB_LIBTINYRAD_LNET_C 1
#include "lnet.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "lurl.h"


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

int
tinyrad_resolve(
         const char *                  hostname,
         int                           port,
         struct sockaddr_storage ***   sa_storagepp,
         size_t *                      sa_storage_lenp,
         uint32_t                      opts )
{
   int                           ai_family;
   int                           ai_flags;
   int                           rc;
   size_t                        size;
   struct addrinfo               hints;
   struct addrinfo *             hintsp;
   struct addrinfo *             res;
   struct addrinfo *             next;
   void *                        ptr;
   size_t                        sas_len;
   struct sockaddr_storage **    sasp;

   assert(hostname     != NULL);
   assert(sa_storagepp != NULL);

   ai_flags = ((opts & TRAD_SERVER)) ? (AI_NUMERICHOST | AI_NUMERICSERV) : 0;
   switch(opts & TRAD_IP_UNSPEC)
   {
      case 0:
      ai_family  = PF_UNSPEC;
      ai_flags  |= AI_ADDRCONFIG;
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

   bzero(&hints, sizeof(struct addrinfo));
   hints.ai_flags    = ai_flags;
   hints.ai_family   = ai_family;
   hints.ai_socktype = ((opts & TRAD_TCP) == 0) ? SOCK_DGRAM  : SOCK_STREAM;
   hints.ai_protocol = ((opts & TRAD_TCP) == 0) ? IPPROTO_UDP : IPPROTO_TCP;
   hintsp            = &hints;

   if ((rc = getaddrinfo(hostname, NULL, hintsp, &res)) != 0)
   {
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(rc));
      return(1);
   };

   // initialize memory
   sas_len = ((sa_storage_lenp)) ? *sa_storage_lenp : 0;
   if ((sasp = *sa_storagepp) == NULL)
   {
      if ((sasp = malloc(sizeof(struct sockaddr_storage *))) == NULL)
         return(TRAD_ENOMEM);
      sasp[0]       = NULL;
      *sa_storagepp = sasp;
      sas_len       = 0;
      if ((sa_storage_lenp))
         *sa_storage_lenp = 0;
   };

   next = res;
   while((next))
   {
      size = sizeof(struct sockaddr_storage *) * (sas_len+2);
      if ((ptr = realloc(sasp, size)) == NULL)
         return(TRAD_ENOMEM);
      sasp            = ptr;
      *sa_storagepp   = ptr;
      sasp[sas_len+1] = NULL;

      if ((sasp[sas_len] = malloc(sizeof(struct sockaddr_storage))) == NULL)
         return(TRAD_ENOMEM);
      bzero(sasp[sas_len],  sizeof(struct sockaddr_storage));
      memcpy(sasp[sas_len], next->ai_addr, next->ai_addrlen);

      switch(sasp[sas_len]->ss_family)
      {
         case AF_INET:
         ((struct sockaddr_in *)sasp[sas_len])->sin_port = htons((uint16_t)port);
         break;

         case AF_INET6:
         ((struct sockaddr_in6 *)sasp[sas_len])->sin6_port = htons((uint16_t)port);
         break;

         default:
         break;
      };

      sas_len++;
      if ((sa_storage_lenp))
         *sa_storage_lenp = sas_len;

      next = next->ai_next;
   };

   freeaddrinfo(res);

   return(TRAD_SUCCESS);
}

/* end of source */
