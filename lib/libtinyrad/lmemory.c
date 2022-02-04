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
#define _LIB_LIBTINYRAD_LMEMORY_C 1
#include "lmemory.h"


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


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#define TRAD_SOCKET_BIND_ADDRESSES_LEN (INET6_ADDRSTRLEN+INET6_ADDRSTRLEN+2)


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//----------------//
// misc functions //
//----------------//
#pragma mark misc functions

int
tinyrad_set_option_socket_bind_addresses(
         TinyRad *                     tr,
         const char *                  invalue );


//------------------//
// object functions //
//------------------//
#pragma mark object functions

int
tinyrad_verify_is_obj(
         void *                     ptr );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

//----------------//
// misc functions //
//----------------//
#pragma mark misc functions

/// destroy Tiny RADIUS reference
///
/// @param[in]  tr            dictionary reference
void
tinyrad_destroy(
         TinyRad *                     tr )
{
   TinyRadDebugTrace();

   assert(tr != NULL);

   if ((tr->trud))
      tinyrad_urldesc_free(tr->trud);
   tr->trud = NULL;

   if ((tr->bind_sa))
      free(tr->bind_sa);
   tr->bind_sa = NULL;

   if ((tr->bind_sa6))
      free(tr->bind_sa6);
   tr->bind_sa6 = NULL;

   if ((tr->net_timeout))
      free(tr->net_timeout);
   tr->net_timeout = NULL;

   if (tr->s != -1)
      close(tr->s);
   tr->s = -1;

   memset(tr, 0, sizeof(TinyRad));
   free(tr);

   return;
}


int
tinyrad_get_option(
         TinyRad *                     tr,
         int                           option,
         void *                        outvalue )
{
   char        bind_buff[TRAD_SOCKET_BIND_ADDRESSES_LEN];

   TinyRadDebugTrace();

   assert(outvalue != NULL);

   // get global options
   switch(option)
   {
      case TRAD_OPT_DEBUG_IDENT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_IDENT, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", tinyrad_debug_ident);
      if ((*((char **)outvalue) = strdup(tinyrad_debug_ident)) == NULL)
         return(TRAD_ENOMEM);
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_LEVEL:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_LEVEL, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: 0x%08x", tinyrad_debug_level);
      *((int *)outvalue) = tinyrad_debug_level;
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_SYSLOG:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_SYSLOG, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", ((tinyrad_debug_syslog)) ? "TRAD_ON" : "TRAD_OFF");
      *((int *)outvalue) = tinyrad_debug_syslog;
      return(TRAD_SUCCESS);

      default:
      break;
   };

   assert(tr!= NULL);

   // get instance options
   switch(option)
   {
      case TRAD_OPT_DESC:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DESC, outvalue )", __func__);
      *((int *)outvalue) = tr->s;
      break;

      case TRAD_OPT_DIAGNOSTIC_MESSAGE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DIAGNOSTIC_MESSAGE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", "unknown");
      if ((*((char **)outvalue) = strdup("unknown")) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_IPV4:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV4, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", ((tr->opts & TRAD_IPV4)) ? TRAD_ON : TRAD_OFF);
      *((int *)outvalue) = ((tr->opts & TRAD_IPV4)) ? TRAD_ON : TRAD_OFF;
      break;

      case TRAD_OPT_IPV6:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV6, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", ((tr->opts & TRAD_IPV6)) ? TRAD_ON : TRAD_OFF);
      *((int *)outvalue) = ((tr->opts & TRAD_IPV6)) ? TRAD_ON : TRAD_OFF;
      break;

      case TRAD_OPT_NETWORK_TIMEOUT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_NETWORK_TIMEOUT, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: tv_sec: %i", tr->net_timeout->tv_sec);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: tv_usec: %i", tr->net_timeout->tv_usec);
      ((struct timeval *)outvalue)->tv_sec   = tr->net_timeout->tv_sec;
      ((struct timeval *)outvalue)->tv_usec  = tr->net_timeout->tv_usec;
      break;

      case TRAD_OPT_SCHEME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SCHEME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", tr->scheme);
      *((unsigned *)outvalue) = tr->scheme;
      break;

      case TRAD_OPT_SOCKET_BIND_ADDRESSES:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, outvalue )", __func__);
      inet_ntop(AF_INET, &tr->bind_sa->sin_addr, bind_buff, sizeof(struct sockaddr_in));
      strncat(bind_buff, " ", (sizeof(bind_buff)-strlen(bind_buff)-1));
      inet_ntop(AF_INET6, &tr->bind_sa6->sin6_addr, &bind_buff[strlen(bind_buff)], sizeof(struct sockaddr_in6));
      if ((*((char **)outvalue) = strdup(bind_buff)) == NULL)
         return(TRAD_ENOMEM);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", bind_buff);
      break;

      case TRAD_OPT_TIMEOUT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_TIMEOUT, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", tr->timeout);
      *((int *)outvalue) = tr->timeout;
      break;

      case TRAD_OPT_URI:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_URI, outvalue )", __func__);
      if (((*(char **)outvalue) = tinyrad_urldesc2str(tr->trud)) == NULL)
         return(TRAD_ENOMEM);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", *(char **)outvalue);
      break;

      default:
      return(TRAD_EOPTERR);
   };

   return(TRAD_SUCCESS);
}


/// initialize Tiny RADIUS reference
///
/// @param[out] trp           pointer to Tiny RADIUS reference
/// @param[in]  url           RADIUS server
/// @param[in]  opts          initialization options
/// @return returns error code
int
tinyrad_initialize(
         TinyRad **                    trp,
         const char *                  url,
         uint64_t                      opts )
{
   TinyRad *         tr;
   int               rc;
   int               fd;
   uint32_t          u32;
   struct timespec   ts;

   TinyRadDebugTrace();

   assert(trp    != NULL);
   assert(url    != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( \"%s\", 0x08x )", __func__, url, opts);

   if ((tr = malloc(sizeof(TinyRad))) == NULL)
      return(TRAD_ENOMEM);
   memset(tr, 0, sizeof(TinyRad));
   tr->opts       = (uint32_t)(opts & TRAD_OPTS_USER);
   tr->s          = -1;
   tr->timeout    = TRAD_DFLT_TIMEOUT;

   // sets bind addresses
   if ((tr->bind_sa = malloc(sizeof(struct sockaddr_in))) == NULL)
   {
      tinyrad_destroy(tr);
      return(TRAD_ENOMEM);
   };
   memset(tr->bind_sa, 0, sizeof(struct sockaddr_in));
   tr->bind_sa->sin_family = AF_INET;
   if ((tr->bind_sa6 = malloc(sizeof(struct sockaddr_in6))) == NULL)
   {
      tinyrad_destroy(tr);
      return(TRAD_ENOMEM);
   };
   memset(tr->bind_sa6, 0, sizeof(struct sockaddr_in6));
   tr->bind_sa6->sin6_family = AF_INET6;
   tinyrad_set_option_socket_bind_addresses(tr, NULL);

   // sets network timeout
   if ((tr->net_timeout = malloc(sizeof(struct timeval))) == NULL)
   {
      tinyrad_destroy(tr);
      return(TRAD_ENOMEM);
   };
   memset(tr->net_timeout, 0, sizeof(struct timeval));
   tr->net_timeout->tv_sec  = TRAD_DFLT_NET_TIMEOUT_SEC;
   tr->net_timeout->tv_usec = TRAD_DFLT_NET_TIMEOUT_USEC;

   // generates initial authenticator
   if ((fd = open("/dev/urandom", O_RDONLY)) != -1)
   {
      read(fd, &u32, sizeof(u32));
      close(fd);
   } else {
      u32 = 0;
      clock_gettime(CLOCK_REALTIME, &ts);
      u32 += (ts.tv_sec  % 0xffffffff);
      u32 += (ts.tv_nsec % 0xffffffff);
      clock_gettime(CLOCK_MONOTONIC, &ts);
      u32 += (ts.tv_sec  % 0xffffffff);
      u32 += (ts.tv_nsec % 0xffffffff);
      u32 = ((u32 & 0xffff0000) >> 16) | ((u32 & 0x0000ffff) << 16);
      u32 = ((u32 & 0xff00ff00) >>  8) | ((u32 & 0x00ff00ff) <<  8);
      u32 = ((u32 & 0xf0f0f0f0) >>  4) | ((u32 & 0x0f0f0f0f) <<  4);
      u32 = ((u32 & 0xcccccccc) >>  2) | ((u32 & 0x33333333) <<  2);
      u32 = ((u32 & 0xaaaaaaaa) >>  1) | ((u32 & 0x55555555) <<  1);
      clock_gettime(CLOCK_REALTIME, &ts);
      u32 += (ts.tv_sec  % 0xffffffff);
      u32 += (ts.tv_nsec % 0xffffffff);
      clock_gettime(CLOCK_MONOTONIC, &ts);
      u32 += (ts.tv_sec  % 0xffffffff);
      u32 += (ts.tv_nsec % 0xffffffff);
   };
   tr->authenticator = u32;

   // parses URL
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_URI, url)) != TRAD_SUCCESS)
   {
      tinyrad_destroy(tr);
      return(rc);
   };

   *trp = tr;

   return(TRAD_SUCCESS);
}


int
tinyrad_set_option(
         TinyRad *                     tr,
         int                           option,
         const void *                  invalue )
{
   uint32_t             opts;
   int                  rc;
   TinyRadURLDesc *     trud;

   TinyRadDebugTrace();

   assert(invalue != NULL);

   // set global options
   switch(option)
   {
      case TRAD_OPT_DEBUG_IDENT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_IDENT, \"%s\" )", __func__, ((!((const char *)invalue)) ? "(NULL)" : (const char *)invalue));
      if (!((const char *)invalue))
         invalue = TRAD_DFLT_DEBUG_IDENT;
      tinyrad_strlcpy(tinyrad_debug_ident_buff, (const char *)invalue, sizeof(tinyrad_debug_ident_buff));
      tinyrad_debug_ident = tinyrad_debug_ident_buff;
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_LEVEL:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_LEVEL, 0x%08x )", __func__, *((const int *)invalue));
      tinyrad_debug_level = *((const int *)invalue);
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_SYSLOG:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DEBUG_SYSLOG, %i )", __func__, *((const int *)invalue));
      switch(*((const int *)invalue))
      {
         case TRAD_ON:  tinyrad_debug_syslog = TRAD_ON;  break;
         case TRAD_OFF: tinyrad_debug_syslog = TRAD_OFF; break;
         default: return(TRAD_EOPTERR);
      };
      return(TRAD_SUCCESS);

      default:
      break;
   };

   assert(tr != NULL);

   // set instance options
   switch(option)
   {
      case TRAD_OPT_DESC:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DESC, invalue )", __func__);
      return(TRAD_EOPTERR);

      case TRAD_OPT_DIAGNOSTIC_MESSAGE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DIAGNOSTIC_MESSAGE, invalue )", __func__);
      return(TRAD_EOPTERR);

      case TRAD_OPT_IPV4:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV4, %s )", __func__, (((*((const int *)invalue))) ? "TRAD_ON" : "TRAD_OFF"));
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      opts = tr->opts;
      if ((*((const int *)invalue)))
         tr->opts |= TRAD_IPV4;
      else
         tr->opts &= ~TRAD_IPV4;
      if (opts == tr->opts)
         break;
      if ((rc = tinyrad_urldesc_resolve(tr->trud, tr->opts)) != TRAD_SUCCESS)
         return(rc);
      break;

      case TRAD_OPT_IPV6:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV6, %s )", __func__, (((*((const int *)invalue))) ? "TRAD_ON" : "TRAD_OFF"));
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      opts = tr->opts;
      if ((*((const int *)invalue)))
         tr->opts |= TRAD_IPV6;
      else
         tr->opts &= ~TRAD_IPV6;
      if (opts == tr->opts)
         break;
      if ((rc = tinyrad_urldesc_resolve(tr->trud, tr->opts)) != TRAD_SUCCESS)
         return(rc);
      break;

      case TRAD_OPT_NETWORK_TIMEOUT:
      memcpy(tr->net_timeout, ((const struct timeval *)invalue), sizeof(struct timeval));
      break;

      case TRAD_OPT_SCHEME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SCHEME, invalue )", __func__);
      return(TRAD_EOPTERR);

      case TRAD_OPT_SOCKET_BIND_ADDRESSES:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, invalue )", __func__);
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      return(tinyrad_set_option_socket_bind_addresses(tr, invalue));

      case TRAD_OPT_TIMEOUT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_TIMEOUT, %i )", __func__, *((const int *)invalue));
      tr->timeout = *((const int *)invalue);
      break;

      case TRAD_OPT_URI:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_URI, \"%s\" )", __func__, (const char *)invalue);
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      if ((rc = tinyrad_urldesc_parse((const char *)invalue, &trud)) != TRAD_SUCCESS)
         return(rc);
      if ((rc = tinyrad_urldesc_resolve(trud, tr->opts)) != TRAD_SUCCESS)
      {
         tinyrad_urldesc_free(trud);
         return(rc);
      };
      tinyrad_urldesc_free(tr->trud);
      tr->scheme  = tinyrad_urldesc_scheme(trud);
      tr->trud    = trud;
      break;

      default:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, %i, invalue )", __func__, option);
      return(TRAD_EOPTERR);
   };

   return(TRAD_SUCCESS);
}


int
tinyrad_set_option_socket_bind_addresses(
         TinyRad *                     tr,
         const char *                  invalue )
{
   char                    buff[TRAD_SOCKET_BIND_ADDRESSES_LEN*2];
   char *                  str;
   char *                  ptr;
   struct sockaddr_in      sa;
   struct sockaddr_in6     sa6;
   struct addrinfo         hints;
   struct addrinfo *       res;
   struct addrinfo *       next;

   TinyRadDebugTrace();

   assert(tr != NULL);

   invalue = (((invalue)) ? invalue : TRAD_DFLT_SOCKET_BIND_ADDRESSES);
   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, \"%s\" )", __func__);
   tinyrad_strlcpy(buff, invalue, sizeof(buff));
   if ((strcmp(invalue, buff)))
      return(TRAD_EOPTERR);

   memset(&sa,  0, sizeof(struct sockaddr_in));
   memset(&sa6, 0, sizeof(struct sockaddr_in6));
   sa.sin_family   = AF_INET;
   sa6.sin6_family = AF_INET6;

   str = buff;
   while( ((str)) && ((str[0])) )
   {
      if ((ptr = strchr(str, ' ')) != NULL)
      {
         ptr[0] = '\0';
         ptr = &ptr[1];
      };
      if (strlen(str) < 2)
         continue;

      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_flags    = AI_NUMERICHOST | AI_NUMERICSERV;
      hints.ai_family   = PF_UNSPEC;

      if (getaddrinfo(str, NULL, &hints, &res) != 0)
         continue;

      next = res;
      while((next))
      {
         switch(next->ai_family)
         {
            case AF_INET:
            if (sizeof(struct sockaddr_in) != next->ai_addrlen)
               return(TRAD_EOPTERR);
            memcpy(&sa, next->ai_addr, next->ai_addrlen);
            sa.sin_port = 0;
            break;

            case AF_INET6:
            if (sizeof(struct sockaddr_in6) != next->ai_addrlen)
               return(TRAD_EOPTERR);
            memcpy(&sa6, next->ai_addr, next->ai_addrlen);
            sa6.sin6_port = 0;
            break;

            default:
            return(TRAD_EOPTERR);
         };
         next = next->ai_next;
      };
      str = ptr;
   };

   memcpy(tr->bind_sa,  &sa,  sizeof(struct sockaddr_in));
   memcpy(tr->bind_sa6, &sa6, sizeof(struct sockaddr_in6));

   return(TRAD_SUCCESS);
}


//------------------//
// object functions //
//------------------//
#pragma mark object functions

void
tinyrad_free(
         void *                        ptr )
{
   intptr_t    ref_count;
   TinyRadDebugTrace();
   if (tinyrad_verify_is_obj(ptr) == TRAD_NO)
   {
      free(ptr);
      return;
   };
   ref_count = atomic_fetch_sub(&((TinyRadObj *)ptr)->ref_count, 1);
   if (ref_count > 1)
      return;
   if (( ((TinyRadObj *)ptr)->free_func ))
      ((TinyRadObj *)ptr)->free_func(ptr);
   free(ptr);
   return;
}


void *
tinyrad_obj_alloc(
         size_t                        size,
         void (*free_func)(void * ptr) )
{
   TinyRadObj *      obj;
   TinyRadDebugTrace();
   assert(size > sizeof(TinyRadObj));
   if ((obj = malloc(size)) == NULL)
      return(NULL);
   memset(obj, 0, size);
   memcpy(obj->magic_header, TRAD_MAGIC, 8);
   atomic_init(&obj->ref_count, 1);
   obj->free_func = free_func;
   return(obj);
}


void *
tinyrad_obj_retain(
         void *                        ptr )
{
   TinyRadDebugTrace();
   if (ptr == NULL)
      return(NULL);
   assert(tinyrad_verify_is_obj(ptr) == TRAD_YES);
   atomic_fetch_add(&((TinyRadObj *)ptr)->ref_count, 1);
   return(ptr);
}


int
tinyrad_verify_is_obj(
         void *                     ptr )
{
   size_t   pos;
   if (!(ptr))
      return(TRAD_NO);
   for(pos = 0; (pos < 8); pos++)
      if (((TinyRadObj *)ptr)->magic_header[pos] != TRAD_MAGIC[pos])
         return(TRAD_NO);
   return(TRAD_YES);
}


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
         dst[pos+offset] = dst[offset];
   len = offset + pos;
   dst[((len < dstsize) ? len : (dstsize-1))] = '\0';
   return(len);
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

/* end of source */
