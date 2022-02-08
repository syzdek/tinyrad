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
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "lurl.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tinyrad_socket_open_socket(
         TinyRad *                     tr,
         struct sockaddr_storage *     sa );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tinyrad_socket_close(
         TinyRad *                     tr )
{
   TinyRadDebugTrace();

   assert(tr != NULL);

   tr->trud_cur = NULL;
   tr->trud_pos = 0;

   if (tr->s != -1)
      close(tr->s);
   tr->s = -1;

   return(TRAD_SUCCESS);
}


int
tinyrad_socket_open(
         TinyRad *                     tr )
{
   TinyRadURLDesc *     trud;

   TinyRadDebugTrace();

   assert(tr != NULL);

   if (tr->s != -1)
      return(TRAD_SUCCESS);

   tr->trud_cur = tr->trud;

   while ((tr->trud_cur))
   {
      trud = tr->trud_cur;

      for(tr->trud_pos = 0; (tr->trud_pos < trud->sockaddrs_len); tr->trud_pos++)
         if (trud->sockaddrs[tr->trud_pos]->ss_family != AF_INET6)
            if (tinyrad_socket_open_socket(tr, trud->sockaddrs[tr->trud_pos]) == TRAD_SUCCESS)
               return(TRAD_SUCCESS);

      for(tr->trud_pos = 0; (tr->trud_pos < trud->sockaddrs_len); tr->trud_pos++)
         if (trud->sockaddrs[tr->trud_pos]->ss_family == AF_INET)
            if (tinyrad_socket_open_socket(tr, trud->sockaddrs[tr->trud_pos]) == TRAD_SUCCESS)
               return(TRAD_SUCCESS);

      tr->trud_pos = 0;
      tr->trud_cur = tr->trud_cur->trud_next;
   };

   tr->trud_cur = NULL;

   return(TRAD_ECONNECT);
}


int
tinyrad_socket_open_socket(
         TinyRad *                     tr,
         struct sockaddr_storage *     sa )
{
   int                  s;
   int                  opt;
   int                  domain;
   int                  type;
   int                  protocol;
   socklen_t            sa_len;
   struct sockaddr *    bind_sa;

   TinyRadDebugTrace();

   assert(tr != NULL);
   assert(sa != NULL);

   type     = ((tr->opts & TRAD_TCP))        ? SOCK_STREAM : SOCK_DGRAM;
   protocol = ((tr->opts & TRAD_TCP))        ? IPPROTO_TCP : IPPROTO_UDP;
   domain   = sa->ss_family;
   sa_len   = (sa->ss_family == AF_INET)     ? sizeof(struct sockaddr_in)     : sizeof(struct sockaddr_in6);
   bind_sa  = (sa->ss_family == AF_INET)     ? (struct sockaddr *)tr->bind_sa : (struct sockaddr *)tr->bind_sa6;

   if ((s = socket(domain, type, protocol)) == -1)
      return(TRAD_ECONNECT);

#ifdef SO_NOSIGPIPE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (void *)&opt, sizeof(int));
#endif
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(int));
   opt = 1;
   if ((tr->opts & TRAD_TCP))
      setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(int));

   if (bind(s, bind_sa, sa_len) == -1)
   {
      close(s);
      return(TRAD_ECONNECT);
   };

   if (connect(s, (struct sockaddr *)sa, sa_len))
   {
      close(s);
      return(TRAD_ECONNECT);
   };

   tr->s = s;

   return(TRAD_SUCCESS);
}


int
tinyrad_socket_reopen(
         TinyRad *                     tr,
         int                           force )
{
   TinyRadURLDesc *     trud;
   int                  loop;

   TinyRadDebugTrace();

   assert(tr != NULL);

   if (!(tr->trud))
      return(TRAD_ECONNECT);
   if ( (!(force)) && (tr->s != -1) )
      return(TRAD_SUCCESS);

   // close existing socket
   if (tr->s != -1)
      close(tr->s);
   tr->s = -1;

   // initialize loop parameters
   if (!(tr->trud_cur))
   {
      tr->trud_cur = tr->trud;
      tr->trud_pos = 0;
   };
   if (tr->trud_pos >= tr->trud_cur->sockaddrs_len)
   {
      tr->trud_cur = tr->trud;
      tr->trud_pos = 0;
   };
   if (!(tr->trud_cur))
   {
      tr->trud_cur = tr->trud;
      tr->trud_pos = 0;
   };

   for(loop = 0; (loop < 2); loop++)
   {
      trud = tr->trud_cur;

      for(; (tr->trud_pos < trud->sockaddrs_len); tr->trud_pos++)
         if (tinyrad_socket_open_socket(tr, trud->sockaddrs[tr->trud_pos]) == TRAD_SUCCESS)
            return(TRAD_SUCCESS);

      tr->trud_pos = 0;
      tr->trud_cur = ((trud->trud_next)) ? trud->trud_next : tr->trud;
   };

   return(TRAD_ECONNECT);
}


/* end of source */
