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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

/// destroy Tiny RADIUS reference
///
/// @param[in]  tr            dictionary reference
void
tinyrad_destroy(
         TinyRad *                     tr )
{
   assert(tr != NULL);

   if ((tr->trud))
      tinyrad_urldesc_free(tr->trud);
   tr->trud = NULL;

   if ((tr->net_timeout))
      free(tr->net_timeout);
   tr->net_timeout = NULL;

   if (tr->s != -1)
      close(tr->s);
   tr->s = -1;

   bzero(tr, sizeof(TinyRad));
   free(tr);

   return;
}


void
tinyrad_free(
         void *                        ptr )
{
   free(ptr);
   return;
}


int
tinyrad_get_option(
         TinyRad *                     tr,
         int                           option,
         void *                        outvalue )
{
   assert(outvalue != NULL);

   // get global options
   switch(option)
   {
      case TRAD_OPT_DEBUG_IDENT:
      if ((*((char **)outvalue) = strdup(tinyrad_debug_ident)) == NULL)
         return(TRAD_ENOMEM);
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_LEVEL:
      *((int *)outvalue) = tinyrad_debug_level;
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_SYSLOG:
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
      *((int *)outvalue) = tr->s;
      break;

      case TRAD_OPT_URI:
      if ((*((char **)outvalue) = tinyrad_urldesc2str(tr->trud)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_ADDRESS_FAMILY:
      *((int *)outvalue) = tr->opts & TRAD_IP_UNSPEC;
      break;

      case TRAD_OPT_DIAGNOSTIC_MESSAGE:
      if ((*((char **)outvalue) = strdup("unknown")) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_NETWORK_TIMEOUT:
      if ((*((struct timeval **)outvalue) = malloc(sizeof(struct timeval))) == NULL)
         return(TRAD_ENOMEM);
      memcpy(*((struct timeval **)outvalue), tr->net_timeout, sizeof(struct timeval));
      break;

      case TRAD_OPT_SOCKET_BIND_ADDRESSES:
      return(TRAD_EOPTERR);

      case TRAD_OPT_TIMEOUT:
      *((int *)outvalue) = tr->timeout;
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
   TinyRad   * tr;
   int         rc;

   assert(trp    != NULL);
   assert(url    != NULL);

   if ((tr = malloc(sizeof(TinyRad))) == NULL)
      return(TRAD_ENOMEM);
   bzero(tr, sizeof(TinyRad));
   tr->opts       = (uint32_t)(opts & TRAD_OPTS_USER);
   tr->s          = -1;
   tr->timeout    = TRAD_DFLT_TIMEOUT;

   // sets network timeout
   if ((tr->net_timeout = malloc(sizeof(struct timeval))) == NULL)
   {
      tinyrad_destroy(tr);
      return(TRAD_ENOMEM);
   };
   bzero(tr->net_timeout, sizeof(struct timeval));
   tr->net_timeout->tv_sec  = TRAD_DFLT_NET_TIMEOUT_SEC;
   tr->net_timeout->tv_usec = TRAD_DFLT_NET_TIMEOUT_USEC;

   // parses URL
   if ((rc = tinyrad_urldesc_parse(url, &tr->trud)) != TRAD_SUCCESS)
   {
      tinyrad_destroy(tr);
      return(rc);
   };
   if ((rc = tinyrad_urldesc_resolve(tr->trud, (uint32_t)opts)) != TRAD_SUCCESS)
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
   int                  rc;
   TinyRadURLDesc *     trud;

   assert(invalue != NULL);

   // set global options
   switch(option)
   {
      case TRAD_OPT_DEBUG_IDENT:
      if (!((const char *)invalue))
         invalue = TRAD_DFLT_DEBUG_IDENT;
      strncpy(tinyrad_debug_ident_buff, (const char *)invalue, sizeof(tinyrad_debug_ident_buff));
      tinyrad_debug_ident = tinyrad_debug_ident_buff;
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_LEVEL:
      tinyrad_debug_level = *((const int *)invalue);
      return(TRAD_SUCCESS);

      case TRAD_OPT_DEBUG_SYSLOG:
      switch(*((const int *)invalue))
      {
         case TRAD_OPT_ON:  tinyrad_debug_syslog = TRAD_OPT_ON;  break;
         case TRAD_OPT_OFF: tinyrad_debug_syslog = TRAD_OPT_OFF; break;
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
      return(TRAD_EOPTERR);

      case TRAD_OPT_URI:
      if ((rc = tinyrad_urldesc_parse((const char *)invalue, &trud)) != TRAD_SUCCESS)
         return(rc);
      if ((rc = tinyrad_urldesc_resolve(trud, tr->opts)) != TRAD_SUCCESS)
      {
         tinyrad_urldesc_free(trud);
         return(rc);
      };
      tinyrad_urldesc_free(tr->trud);
      tr->trud = trud;
      if (tr->s != -1)
         close(tr->s);
      tr->s = -1;
      break;

      case TRAD_OPT_ADDRESS_FAMILY:
      if ( (tr->opts & TRAD_IP_UNSPEC) == (*(const int *)invalue) )
         return(TRAD_SUCCESS);
      if (tr->s != -1)
         close(tr->s);
      tr->s    =  -1;
      tr->opts &= ~TRAD_IP_UNSPEC;
      tr->opts |= (*(const int *)invalue) & TRAD_IP_UNSPEC;
      if ((rc = tinyrad_urldesc_resolve(tr->trud, tr->opts)) != TRAD_SUCCESS)
         return(rc);
      break;

      case TRAD_OPT_DIAGNOSTIC_MESSAGE:
      return(TRAD_EOPTERR);

      case TRAD_OPT_NETWORK_TIMEOUT:
      memcpy(tr->net_timeout, ((const struct timeval *)invalue), sizeof(struct timeval));
      break;

      case TRAD_OPT_SOCKET_BIND_ADDRESSES:
      return(TRAD_EOPTERR);

      case TRAD_OPT_TIMEOUT:
      tr->timeout = *((const int *)invalue);
      break;

      default:
      return(TRAD_EOPTERR);
   };

   return(TRAD_SUCCESS);
}


/* end of source */
