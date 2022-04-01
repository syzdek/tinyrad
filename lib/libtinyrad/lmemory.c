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
#include <assert.h>

#include "lconf.h"
#include "ldict.h"
#include "lfile.h"
#include "lstrings.h"


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

//-------------------//
// random prototypes //
//-------------------//
#pragma mark random prototypes

int
tinyrad_srandom(
         TinyRad *                     tr );


int
tinyrad_srandom_seed(
         void *                        dst,
         size_t                        n );


//--------------------//
// TinyRad prototypes //
//--------------------//
#pragma mark TinyRad prototypes

int
tinyrad_set_option_socket_bind_addresses(
         TinyRad *                     tr,
         const char *                  invalue );


int
tinyrad_tiyrad_defaults(
         TinyRad *                     tr,
         unsigned                      opts );


void
tinyrad_tiyrad_free(
         TinyRad *                     tr );


//-------------------//
// object prototypes //
//-------------------//
#pragma mark object prototypes

int
tinyrad_verify_is_obj(
         TinyRadObj *                  obj );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static atomic_flag tinyrad_rand_init = ATOMIC_FLAG_INIT;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

//-------------------------//
// miscellaneous functions //
//-------------------------//
#pragma mark miscellaneous functions

int
tinyrad_set_flag(
         unsigned *                    optsp,
         unsigned *                    opts_negp,
         unsigned                      opt,
         int                           val )
{
   unsigned    opts;
   unsigned    opts_neg;

   opts      = ((optsp))      ? *optsp       :  (0U);
   opts     |= ((val))        ? opt          :  (0U);
   opts     &= (!(val))       ? ~opt         : ~(0U);
   opts_neg  = ((opts_negp))  ? *opts_negp   :  (0U);
   opts_neg |= (!(val))       ? opt          :  (0U);
   opts_neg &= ((val))        ? ~opt         : ~(0U);

   if ((optsp))
      *optsp = opts;
   if ((opts_negp))
      *opts_negp = opts_neg;

   return(TRAD_SUCCESS);
}


//------------------//
// random functions //
//------------------//
#pragma mark random functions

int
tinyrad_random_buf(
         TinyRad *                     tr,
         void *                        buf,
         size_t                        nbytes )
{
   long           rnd;
   size_t         off;
   size_t         size;

   TinyRadDebugTrace();

   // read from random number file
   if (tr->rand != -1)
      if (read(tr->rand, buf, nbytes) == (ssize_t)nbytes)
         return(TRAD_SUCCESS);

   // fall back to random()
   for(off = 0; (off < nbytes); off += sizeof(rnd))
   {
      rnd   = random();
      size  = ((off + sizeof(rnd)) < nbytes) ? sizeof(rnd) : (nbytes - off);
      memcpy(&((uint8_t *)buf)[off], &rnd, size);
   };

   return(TRAD_SUCCESS);
}


int
tinyrad_srandom(
         TinyRad *                     tr )
{
   unsigned       seed;
   const char *   file;

   TinyRadDebugTrace();

   assert(tr != NULL);

   // set default random file option
   if (!(tr->opts & TRAD_RANDOM_MASK))
      tr->opts |= TRAD_URANDOM;

   // closes existing random number file
   if (tr->rand != -1)
      close(tr->rand);
   tr->rand = -1;

   // initialize random number generator
   if (!(atomic_flag_test_and_set(&tinyrad_rand_init)))
   {
      tinyrad_srandom_seed(&seed, sizeof(seed));
      srandom(seed);
   };

   // determine name of random number file
   switch(tr->opts & TRAD_RANDOM_MASK)
   {
      case TRAD_RAND:      return(TRAD_SUCCESS);
      case TRAD_RANDOM:    file = "/dev/random";  break;
      case TRAD_URANDOM:   file = "/dev/urandom"; break;
      default:             return(TRAD_EUNKNOWN);
   };

   // open random number file
   if ((tr->rand = open(file, O_RDONLY)) == -1)
      return(TRAD_EUNKNOWN);

   return(TRAD_SUCCESS);
}


int
tinyrad_srandom_seed(
         void *                        buf,
         size_t                        nbytes )
{
   int               fd;
   ssize_t           rc;
   size_t            off;
   uint32_t          rnd;
   struct timespec   ts;
   size_t            size;

   TinyRadDebugTrace();

   // attempt to use random file
   if ((fd = open("/dev/unrandom", O_RDONLY)) == -1)
      fd = open("/dev/nrandom", O_RDONLY);
   if (fd != -1)
   {
      rc = read(fd, buf, nbytes);
      close(fd);
      if (rc == (ssize_t)nbytes)
         return(TRAD_SUCCESS);
   };

   // fall back to using time
   for(off = 0; (off < nbytes); off += sizeof(rnd))
   {
      // generate a, not random, possibly hard to predict number (there is a
      // reason this is the fall back method and not the primary method for
      // seeding the random number generator).  Better recommendations are
      // welcome.
      rnd = 0;
      // add realtime seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_REALTIME, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
      // add monotonic time seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_MONOTONIC, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
#ifdef _POSIX_CPUTIME
      // add process CPU time seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
#endif
      // reverse bits
      rnd = ((rnd & 0xffff0000) >> 16) | ((rnd & 0x0000ffff) << 16);
      rnd = ((rnd & 0xff00ff00) >>  8) | ((rnd & 0x00ff00ff) <<  8);
      rnd = ((rnd & 0xf0f0f0f0) >>  4) | ((rnd & 0x0f0f0f0f) <<  4);
      rnd = ((rnd & 0xcccccccc) >>  2) | ((rnd & 0x33333333) <<  2);
      rnd = ((rnd & 0xaaaaaaaa) >>  1) | ((rnd & 0x55555555) <<  1);
      // add realtime seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_REALTIME, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
      // add monotonic time seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_MONOTONIC, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
#ifdef _POSIX_CPUTIME
      // add process CPU time seconds and nanoseconds to rnd
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
      rnd += (ts.tv_sec  % 0xffffffff);
      rnd += (ts.tv_nsec % 0xffffffff);
#endif

      size = ((off + sizeof(rnd)) < nbytes) ? sizeof(rnd) : (nbytes - off);
      memcpy(&((uint8_t *)buf)[off], &rnd, size);
   };

   return(TRAD_SUCCESS);
}


//-------------------//
// TinyRad functions //
//-------------------//
#pragma mark TinyRad functions

int
tinyrad_tiyrad_defaults(
         TinyRad *                     tr,
         unsigned                      opts )
{
   int            rc;
   const char *   str;

   TinyRadDebugTrace();

   assert(tr != NULL);

   // sets default URI
   if (!(tr->trud))
      if ((rc = tinyrad_set_option(tr, TRAD_OPT_URI, TRAD_DFLT_URI)) != TRAD_SUCCESS)
         return(rc);

   // sets default bind addresses
   if ( (!(tr->bind_sa)) || (!(tr->bind_sa6)) )
      if ((rc = tinyrad_set_option_socket_bind_addresses(tr, NULL)) != TRAD_SUCCESS)
         return(rc);

   // sets default network timeout
   if (!(tr->net_timeout))
   {
      if ((tr->net_timeout = malloc(sizeof(struct timeval))) == NULL)
         return(TRAD_ENOMEM);
      memset(tr->net_timeout, 0, sizeof(struct timeval));
      tr->net_timeout->tv_sec  = TRAD_DFLT_NET_TIMEOUT_SEC;
      tr->net_timeout->tv_usec = TRAD_DFLT_NET_TIMEOUT_USEC;
   };

   // sets default timeout
   if (tr->timeout == -1)
      tr->timeout = TRAD_DFLT_TIMEOUT;

   // sets secret
   str = ((tr->opts&TRAD_CATHOLIC)) ? "Dominus vobiscum" : "tinyrad";
   if (!(tr->secret))
      if ((tr->secret = tinyrad_strdup(str)) == NULL)
         return(TRAD_ENOMEM);

   tr->opts |= opts;

   return(TRAD_SUCCESS);
}


/// destroy Tiny RADIUS reference
///
/// @param[in]  tr            dictionary reference
void
tinyrad_tiyrad_free(
         TinyRad *                     tr )
{
   TinyRadDebugTrace();

   assert(tr != NULL);

   if ((tr->dict))
      tinyrad_obj_release(&tr->dict->obj);

   if ((tr->secret))
      free(tr->secret);

   if ((tr->secret_file))
      free(tr->secret_file);

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

   if (tr->rand != -1)
      close(tr->rand);
   tr->rand = -1;

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
      if ((*((char **)outvalue) = tinyrad_strdup(tinyrad_debug_ident)) == NULL)
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
      if ((*((char **)outvalue) = tinyrad_strdup("unknown")) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_DICTIONARY:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DICTIONARY, outvalue )", __func__);
      *((TinyRadDict **)outvalue) = tinyrad_obj_retain(&tr->dict->obj);
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

      case TRAD_OPT_RANDOM:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_RANDOM, outvalue )", __func__);
      switch(tr->opts & TRAD_RANDOM_MASK)
      {  case TRAD_RAND:    TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", "TRAD_RAND");    break;
         case TRAD_RANDOM:  TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", "TRAD_RANDOM");  break;
         case TRAD_URANDOM: TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", "TRAD_URANDOM"); break;
         default:           TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", (tr->opts & TRAD_RANDOM_MASK)); break;
      };
      *((int *)outvalue) = tr->opts & TRAD_RANDOM_MASK;
      break;

      case TRAD_OPT_SCHEME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SCHEME, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %i", tr->scheme);
      *((unsigned *)outvalue) = tr->scheme;
      break;

      case TRAD_OPT_SECRET:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SECRET, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", ((tr->secret)) ? tr->secret : "(null)");
      *((char **)outvalue) = NULL;
      if ((tr->secret))
         if (((*(char **)outvalue) = tinyrad_strdup(tr->secret)) == NULL)
            return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_SECRET_FILE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SECRET_FILE, outvalue )", __func__);
      TinyRadDebug(TRAD_DEBUG_ARGS, "   <= outvalue: %s", ((tr->secret_file)) ? tr->secret_file : "(null)");
      *((char **)outvalue) = NULL;
      if ((tr->secret_file))
         if (((*(char **)outvalue) = tinyrad_strdup(tr->secret_file)) == NULL)
            return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_SOCKET_BIND_ADDRESSES:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, outvalue )", __func__);
      inet_ntop(AF_INET, &tr->bind_sa->sin_addr, bind_buff, sizeof(struct sockaddr_in));
      strncat(bind_buff, " ", (sizeof(bind_buff)-strlen(bind_buff)-1));
      inet_ntop(AF_INET6, &tr->bind_sa6->sin6_addr, &bind_buff[strlen(bind_buff)], sizeof(struct sockaddr_in6));
      if ((*((char **)outvalue) = tinyrad_strdup(bind_buff)) == NULL)
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
         TinyRadDict *                 dict,
         const char *                  url,
         unsigned                      opts )
{
   TinyRad *         tr;
   int               rc;
   int               opt;

   TinyRadDebugTrace();

   assert(trp    != NULL);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( \"%s\", 0x08x )", __func__, url, opts);

   if ((tr = tinyrad_obj_alloc(sizeof(TinyRad), (void(*)(void*))&tinyrad_tiyrad_free)) == NULL)
      return(TRAD_ENOMEM);
   tr->opts       = (uint32_t)(opts & TRAD_OPTS_USER);
   tr->s          = -1;
   tr->timeout    = -1;
   tr->rand       = -1;

   // parses and saves URL
   if ((url))
   {
      if ((rc = tinyrad_set_option(tr, TRAD_OPT_URI, url)) != TRAD_SUCCESS)
      {
         tinyrad_tiyrad_free(tr);
         return(rc);
      };
   };

   // retain or initialize dictionary
   if ((tr->dict = tinyrad_obj_retain(&dict->obj)) == NULL)
   {
      if ((rc = tinyrad_dict_initialize(&tr->dict, (opts|TRAD_NOINIT))) != TRAD_SUCCESS)
      {
         tinyrad_tiyrad_free(tr);
         return(rc);
      };
   };

   // read init files
   if ((rc = tinyrad_conf(tr, (((dict)) ? NULL : tr->dict), opts)) != TRAD_SUCCESS)
   {
      tinyrad_tiyrad_free(tr);
      return(rc);
   };

   // load builtin dictionary, load default dictionary file, and make read-only
   if (!(dict))
   {
      if ((rc = tinyrad_dict_defaults(tr->dict, NULL, opts)) != TRAD_SUCCESS)
      {
         tinyrad_tiyrad_free(tr);
         return(rc);
      };
      opt = TRAD_YES;
      tinyrad_dict_set_option(tr->dict, TRAD_DICT_OPT_READONLY, &opt);
   };

   // apply defaults
   if ((rc = tinyrad_tiyrad_defaults(tr, opts)) != TRAD_SUCCESS)
   {
      tinyrad_tiyrad_free(tr);
      return(rc);
   };

   // initialize random number generator
   if ((rc = tinyrad_srandom(tr)) != TRAD_SUCCESS)
   {
      tinyrad_tiyrad_free(tr);
      return(rc);
   };

   // generates initial authenticator
   if ((rc = tinyrad_random_buf(tr, &tr->authenticator, sizeof(tr->authenticator))) != TRAD_SUCCESS)
   {
      tinyrad_tiyrad_free(tr);
      return(rc);
   };

   *trp = tinyrad_obj_retain(&tr->obj);

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
   char                 buff[256];

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

      case TRAD_OPT_DICTIONARY:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_DICTIONARY, invalue )", __func__);
      return(TRAD_EOPTERR);

      case TRAD_OPT_IPV4:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV4, %s )", __func__, (((*((const int *)invalue))) ? "TRAD_ON" : "TRAD_OFF"));
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      opts = tr->opts;
      tinyrad_set_flag(&tr->opts, &tr->opts_neg, TRAD_IPV4, *((const int *)invalue) );
      if ( (opts != tr->opts) && ((tr->trud)) )
         if ((rc = tinyrad_urldesc_resolve(tr->trud, tr->opts)) != TRAD_SUCCESS)
            return(rc);
      break;

      case TRAD_OPT_IPV6:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_IPV6, %s )", __func__, (((*((const int *)invalue))) ? "TRAD_ON" : "TRAD_OFF"));
      if (tr->s != -1)
         return(TRAD_EOPTERR);
      opts = tr->opts;
      tinyrad_set_flag(&tr->opts, &tr->opts_neg, TRAD_IPV6, *((const int *)invalue) );
      if ( (opts != tr->opts) && ((tr->trud)) )
         if ((rc = tinyrad_urldesc_resolve(tr->trud, tr->opts)) != TRAD_SUCCESS)
            return(rc);
      break;

      case TRAD_OPT_NETWORK_TIMEOUT:
      memcpy(tr->net_timeout, ((const struct timeval *)invalue), sizeof(struct timeval));
      break;

      case TRAD_OPT_RANDOM:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_RANDOM, invalue )", __func__);
      switch( *((const int *)invalue) & TRAD_RANDOM_MASK)
      {  case TRAD_RAND:    TinyRadDebug(TRAD_DEBUG_ARGS, "   <= invalue: %s", "TRAD_RAND");    break;
         case TRAD_RANDOM:  TinyRadDebug(TRAD_DEBUG_ARGS, "   <= invalue: %s", "TRAD_RANDOM");  break;
         case TRAD_URANDOM: TinyRadDebug(TRAD_DEBUG_ARGS, "   <= invalue: %s", "TRAD_URANDOM"); break;
         default:           TinyRadDebug(TRAD_DEBUG_ARGS, "   <= invalue: %i", *((const int *)invalue)); break;
      };
      opts      =  tr->opts;
      tr->opts &= ~(TRAD_RANDOM_MASK);
      tr->opts |=  (TRAD_RANDOM_MASK & (*((const int *)invalue)));
      if (opts != tr->opts)
         tinyrad_srandom(tr);
      break;

      case TRAD_OPT_SCHEME:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SCHEME, invalue )", __func__);
      return(TRAD_EOPTERR);

      case TRAD_OPT_SECRET:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SECRET, \"%s\" )", __func__, (const char *)invalue);
      if ((tr->secret_file))
         free(tr->secret_file);
      tr->secret_file = NULL;
      if ((tr->secret))
         free(tr->secret);
      if ((tr->secret = tinyrad_strdup(invalue)) == NULL)
         return(TRAD_ENOMEM);
      break;

      case TRAD_OPT_SECRET_FILE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_OPT_SECRET_FILE, \"%s\" )", __func__, (const char *)invalue);
      if ((rc = tinyrad_filetostr(buff, (const char *)invalue, sizeof(buff))) != TRAD_SUCCESS)
         return(rc);
      if ((tr->secret))
         free(tr->secret);
      if ((tr->secret_file))
         free(tr->secret_file);
      tr->secret      = tinyrad_strdup(buff);
      tr->secret_file = tinyrad_strdup((const char *)invalue);
      if ( (!(tr->secret)) || (!(tr->secret_file)) )
         return(TRAD_ENOMEM);
      break;

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

   if (!(tr->bind_sa))
   {
      if ((tr->bind_sa = malloc(sizeof(struct sockaddr_in))) == NULL)
         return(TRAD_ENOMEM);
      memset(tr->bind_sa, 0, sizeof(struct sockaddr_in));
      tr->bind_sa->sin_family = AF_INET;
   };
   if (!(tr->bind_sa6))
   {
      if ((tr->bind_sa6 = malloc(sizeof(struct sockaddr_in6))) == NULL)
         return(TRAD_ENOMEM);
      memset(tr->bind_sa6, 0, sizeof(struct sockaddr_in6));
      tr->bind_sa6->sin6_family = AF_INET6;
   };

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


//---------------------------//
// TinyRadBinValue functions //
//---------------------------//
#pragma mark TinyRadBinValue functions

TinyRadBinValue *
tinyrad_binval_alloc(
         size_t                        size )
{
   TinyRadBinValue *    ptr;
   size_t               adjsize;

   TinyRadDebugTrace();

   adjsize = size + sizeof(TinyRadBinValue);

   if ((ptr = malloc(adjsize)) == NULL)
      return(ptr);
   memset(ptr, 0, adjsize);
   ptr->bv_len = size;

   return(ptr);
}


TinyRadBinValue *
tinyrad_binval_dup(
         const TinyRadBinValue *       ptr )
{
   TinyRadBinValue *    binval;
   size_t               adjsize;

   TinyRadDebugTrace();

   assert(ptr != NULL);

   adjsize = sizeof(TinyRadBinValue) + ptr->bv_len;

   if ((binval = malloc(adjsize)) == NULL)
      return(NULL);
   memcpy(binval, ptr, adjsize);

   return(binval);
}


TinyRadBinValue *
tinyrad_binval_realloc(
         TinyRadBinValue *             ptr,
         size_t                        size )
{
   size_t adjsize;
   TinyRadDebugTrace();
   adjsize = sizeof(TinyRadBinValue) + size;
   if ((ptr = realloc(ptr, adjsize)) == NULL)
      return(ptr);
   ptr->bv_len = size;
   return(ptr);
}


//--------------------------------//
// TinyRadBinValue list functions //
//--------------------------------//
#pragma mark TinyRadBinValue list functions

int
tinyrad_binval_list_add(
         TinyRadBinValue ***           listp,
         const TinyRadBinValue *       val )
{
   TinyRadBinValue **      list;
   size_t                  len;
   size_t                  size;

   TinyRadDebugTrace();

   assert(listp != NULL);
   assert(val   != NULL);

   // calculate current length of list
   len = tinyrad_binval_list_count(*listp);

   // increase size of list
   size = sizeof(TinyRadBinValue *) * (len+2);
   if ((list = realloc(*listp, size)) == NULL)
      return(TRAD_ENOMEM);
   *listp        = list;
   list[len + 1] = NULL;

   // duplicate value
   if ((list[len] = tinyrad_binval_dup(val)) == NULL)
      return(TRAD_ENOMEM);

   return(TRAD_SUCCESS);
}


size_t
tinyrad_binval_list_count(
         TinyRadBinValue **            list )
{
   size_t len;
   TinyRadDebugTrace();
   if (!(list))
      return(0);
   for(len = 0; ((list[len])); len++);
   return(len);
}


void
tinyrad_binval_list_free(
         TinyRadBinValue **            list )
{
   size_t pos;
   TinyRadDebugTrace();
   if (!(list))
      return;
   for(pos = 0; ((list[pos])); pos++)
      tinyrad_free(list[pos]);
   free(list);
   return;
}


//------------------//
// object functions //
//------------------//
#pragma mark object functions

void
tinyrad_free(
         void *                        ptr )
{
   TinyRadDebugTrace();
   if (!(ptr))
      return;
   if (tinyrad_verify_is_obj(ptr) == TRAD_NO)
   {
      free(ptr);
      return;
   };
   tinyrad_obj_release(ptr);
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
   memcpy(obj->magic, TRAD_MAGIC, 8);
   atomic_init(&obj->ref_count, 0);
   obj->free_func = ((free_func)) ? free_func : &free;
   return(obj);
}


void
tinyrad_obj_release(
         TinyRadObj *                  obj )
{
   TinyRadDebugTrace();
   assert(obj != NULL);
   assert(tinyrad_verify_is_obj(obj) == TRAD_YES);
   if (atomic_fetch_sub(&obj->ref_count, 1) > 1)
      return;
   obj->free_func(obj);
   return;
}


void *
tinyrad_obj_retain(
         TinyRadObj *                  obj )
{
   TinyRadDebugTrace();
   if (obj == NULL)
      return(NULL);
   assert(tinyrad_verify_is_obj(obj) == TRAD_YES);
   atomic_fetch_add(&obj->ref_count, 1);
   return(obj);
}


intptr_t
tinyrad_obj_retain_count(
         TinyRadObj *                  obj )
{
   TinyRadDebugTrace();
   if (obj == NULL)
      return(0);
   assert(tinyrad_verify_is_obj(obj) == TRAD_YES);
   return(atomic_fetch_add(&obj->ref_count, 0));
}


int
tinyrad_verify_is_obj(
         TinyRadObj *                  obj )
{
   size_t   pos;
   TinyRadDebugTrace();
   if (!(obj))
      return(TRAD_NO);
   for(pos = 0; (pos < 8); pos++)
      if (obj->magic[pos] != TRAD_MAGIC[pos])
         return(TRAD_NO);
   return(TRAD_YES);
}


/* end of source */
