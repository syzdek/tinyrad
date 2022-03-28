/*
 *  Tiny RADIUS Client Library
 *  Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.
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
#define _SRC_TRU_WIDGET_URL_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "tinyradutil.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tru_widget_url_print(
         TinyRadURLDesc *              trudp );


int
tru_widget_url_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tru_widget_url(
         TinyRadUtilConf *                 cnf )
{
   int                        rc;
   int                        pos;
   TinyRadURLDesc *           trudp;
   TinyRadURLDesc **          trudpp;
   TinyRadURLDesc *           trudp_next;

   // getopt options
   static char          short_opt[] = "r" TRU_COMMON_SHORT TRU_NET_SHORT;
   static struct option long_opt[]  =
   {
      { "resolve",          no_argument,       NULL, 'r' }, \
      TRU_NET_LONG \
      TRU_COMMON_LONG
   };

   trudp       = NULL;
   trudp_next  = NULL;

   // process widget cli options
   if ((rc = tru_cli_parse(cnf, cnf->argc, cnf->argv, short_opt, long_opt, &tru_widget_url_usage)) != 0)
   {
      return((rc == 0) ? 0 : 1);
   };
   if (optind >= cnf->argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
      return(1);
   };

   // parse and resolve URLs
   trudpp = &trudp;
   for(pos = optind; (pos < cnf->argc); pos++)
   {
      if ((rc = tinyrad_urldesc_parse(cnf->argv[pos], trudpp)) != TRAD_SUCCESS)
      {
         if (!(cnf->opts & TRUTILS_OPT_QUIET))
         {
            fprintf(stderr, "%s: %s: %s: %s\n", PROGRAM_NAME, cnf->widget_name, cnf->argv[pos], tinyrad_strerror(rc));
            fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
         };
         return(1);
      };
      if ((cnf->opts & TRU_OPT_RESOLVE))
      {
         if ((rc = tinyrad_urldesc_resolve(*trudpp, cnf->tr_opts)) != TRAD_SUCCESS)
         {
            if (!(cnf->opts & TRUTILS_OPT_QUIET))
            {
               fprintf(stderr, "%s: %s: %s: %s\n", PROGRAM_NAME, cnf->widget_name, cnf->argv[pos], tinyrad_strerror(rc));
               fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
            };
            return(1);
         };
      };
      trudpp = &(*trudpp)->trud_next;
   };

   // exit if not displaying output
   if (!(cnf->opts & TRUTILS_OPT_VERBOSE))
   {
      tinyrad_urldesc_free(trudp);
      return(0);
   };

   // display URLs
   trudp_next = trudp;
   while ((trudp_next))
   {
      if ((tru_widget_url_print(trudp_next)))
      {
         tinyrad_urldesc_free(trudp);
         return(1);
      };
      trudp_next = trudp_next->trud_next;
   };

   tinyrad_urldesc_free(trudp);

   return(TRAD_SUCCESS);
}


int
tru_widget_url_print(
         TinyRadURLDesc *              trudp )
{
   int                        pos;
   char *                     str;
   const char *               scheme;
   struct sockaddr_storage *  sas;
   char                       addr[INET6_ADDRSTRLEN];

   if ((str = tinyrad_urldesc2str(trudp)) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      tinyrad_urldesc_free(trudp);
      return(1);
   };
   switch(trudp->trud_opts & TRAD_SCHEME)
   {
      case TRAD_RADIUS:          scheme = "radius (RFC2865 RADIUS)"; break;;
      case TRAD_RADIUS_ACCT:     scheme = "radius-acct (RFC2866 RADIUS Accounting)"; break;;
      case TRAD_RADIUS_DYNAUTH:  scheme = "radius-dynauth (RFC2866 RADIUS Dynamic Authorization)"; break;;
      case TRAD_RADSEC:          scheme = "radsec (RFC6614/RFC7360 RADIUS TLS/DTLS)"; break;;
      default:                   scheme = "Unknown or unsupported URL scheme";  break;
   };

   printf("%s\n", str);
   printf("   scheme:     %s\n", scheme);
   printf("   host:       %s\n", trudp->trud_host);
   printf("   port:       %i\n", trudp->trud_port);
   printf("   secret:     %s\n", (((trudp->trud_secret)) ? trudp->trud_secret : "n/a"));
   printf("   protocol:   %s\n", ((trudp->trud_opts & TRAD_TCP)) ? "tcp" : "udp");
   if ( ((trudp->trud_sockaddrs)) && ((trudp->trud_sockaddrs[0])) )
   {
      for(pos = 0; ((trudp->trud_sockaddrs[pos])); pos++)
      {
         sas = trudp->trud_sockaddrs[pos];
         switch(sas->ss_family)
         {
            case AF_INET:  inet_ntop(sas->ss_family, &((struct sockaddr_in  *)sas)->sin_addr,  addr, sizeof(addr)); break;
            case AF_INET6: inet_ntop(sas->ss_family, &((struct sockaddr_in6 *)sas)->sin6_addr, addr, sizeof(addr)); break;
            default:
            addr[0] = '\0';
            break;
         };
         printf("   %s%s\n", ((!(pos)) ? "addresses:  " : "            "), addr);
      };
   };
   printf("\n");

   tinyrad_free(str);

   return(0);
}


int
tru_widget_url_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt )
{
   tru_usage_summary(cnf);
   tru_usage_options(short_opt);
   printf("\n");
   printf("URL Syntax:\n");
   printf("  radius://hostport/[secret][?proto]         use RFC2865 (udp/port 1812)\n");
   printf("  radius-acct://hostport/[secret][?proto]    use RFC2866 (udp/port 1813)\n");
   printf("  radsec://hostport/[?proto]                 use RFC6614/RFC7360 (tcp/port 2083)\n");
   printf("  radius-dynauth://hostport/[secret][?proto] use RFC5176 (udp/port 3799)\n");
   printf("\n");
   printf("  where:\n");
   printf("     'hostport' is a host name with an optional ':portnumber'\n");
   printf("     'secret' is the RADIUS secret encoded using character encoding in RFC3986\n");
   printf("     'proto' is communication protocol used by the connection (UDP or TCP)\n");
   printf("\n");
   return(0);
}

/* end of source */
