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
#define _SRC_UTILITIES_TINYRAD_URL_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "common.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-url"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main( int argc, char * argv[] );

void my_usage( void );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int                        c;
   int                        rc;
   int                        pos;
   int                        opt_index;
   int                        quiet;
   int                        resolve;
   int                        verbose;
   char *                     str;
   char                       addr[INET6_ADDRSTRLEN];
   const char *               scheme;
   uint32_t                   opts;
   struct sockaddr_storage *  sas;
   TinyRadURLDesc *           trudp;
   TinyRadURLDesc **          trudpp;
   TinyRadURLDesc *           trudp_next;

   // getopt options
   static char          short_opt[] = "46hqrVv";
   static struct option long_opt[] =
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"resolve",          no_argument,       NULL, 'r' },
      { NULL, 0, NULL, 0 }
   };

   opts        = 0;
   quiet       = 0;
   resolve     = 0;
   verbose     = 0;
   trudp       = NULL;
   trudp_next  = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case '4':
         opts |= TRAD_IPV4;
         break;

         case '6':
         opts |= TRAD_IPV6;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'q':
         quiet = 1;
         break;

         case 'r':
         resolve = 1;
         break;

         case 'V':
         our_version(PROGRAM_NAME);
         return(0);

         case 'v':
         verbose++;
         break;

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   if ( ((quiet)) && ((verbose)) )
   {
      fprintf(stderr, "%s: incompatible options `--silent' and `--verbose'\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   for(pos = optind; (pos < argc); pos++)
   {
      if ((rc = tinyrad_is_radius_url(argv[pos])) != TRAD_SUCCESS)
      {
         if (!(quiet))
         {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, argv[pos], tinyrad_strerror(rc));
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         };
         return(1);
      };
   };

   trudpp = &trudp;
   for(pos = optind; (pos < argc); pos++)
   {
      if ((rc = tinyrad_urldesc_parse(argv[pos], trudpp)) != TRAD_SUCCESS)
      {
         if (!(quiet))
         {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, argv[pos], tinyrad_strerror(rc));
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         };
         return(1);
      };
      if ((resolve))
      {
         if ((rc = tinyrad_urldesc_resolve(*trudpp, opts)) != TRAD_SUCCESS)
         {
            if (!(quiet))
            {
               fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, argv[pos], tinyrad_strerror(rc));
               fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            };
            return(1);
         };
      };
      trudpp = &(*trudpp)->trud_next;
   };

   if (!(verbose))
   {
      tinyrad_urldesc_free(trudp);
      return(0);
   };

   trudp_next = trudp;
   while ((trudp_next))
   {
      if ((str = tinyrad_urldesc2str(trudp_next)) == NULL)
      {
         fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
         tinyrad_urldesc_free(trudp);
         return(1);
      };
      switch(trudp_next->trud_opts & TRAD_SCHEME)
      {
         case TRAD_RADIUS:          scheme = "radius (RFC2865 RADIUS)"; break;;
         case TRAD_RADIUS_ACCT:     scheme = "radius-acct (RFC2866 RADIUS Accounting)"; break;;
         case TRAD_RADIUS_DYNAUTH:  scheme = "radius-dynauth (RFC2866 RADIUS Dynamic Authorization)"; break;;
         case TRAD_RADSEC:          scheme = "radsec (RFC6614/RFC7360 RADIUS TLS/DTLS)"; break;;
         default:                   scheme = "Unknown or unsupported URL scheme";  break;
      };
      printf("%s\n", str);
      printf("   scheme:     %s\n", scheme);
      printf("   host:       %s\n", trudp_next->trud_host);
      if ( ((trudp_next->sockaddrs)) && ((trudp_next->sockaddrs[0])) )
      {
         for(pos = 0; ((trudp_next->sockaddrs[pos])); pos++)
         {
            sas = trudp_next->sockaddrs[pos];
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
      printf("   port:       %i\n", trudp_next->trud_port);
      printf("   secret:     %s\n", trudp_next->trud_secret);
      printf("   protocol:   %s\n", ((trudp_next->trud_opts & TRAD_TCP)) ? "tcp" : "udp");
      printf("\n");
      tinyrad_free(str);
      trudp_next = trudp_next->trud_next;
   };

   tinyrad_urldesc_free(trudp);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] url url ... url\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -h, --help                                print this help and exit\n");
   printf("  -q, --quiet, --silent                     do not print messages\n");
   printf("  -V, --version                             print version number and exit\n");
   printf("  -v, --verbose                             print verbose messages\n");
   printf("  -r, --resolve                             resolve URLs\n");
   printf("\n");
   printf("URL Syntax:\n");
   printf("  radius://hostport/secret[?proto]          use RFC2865 (udp/port 1812)\n");
   printf("  radius-acct://hostport/secret[?proto]     use RFC2866 (udp/port 1813)\n");
   printf("  radsec://hostport/[?proto]                use RFC6614/RFC7360 (tcp/port 2083)\n");
   printf("  radius-dynauth://hostport/secret[?proto]  use RFC5176 (udp/port 3799)\n");
   printf("\n");
   printf("  where:\n");
   printf("     'hostport' is a host name with an optional ':portnumber'\n");
   printf("     'secret' is the RADIUS secret encoded using character encoding in RFC3986\n");
   printf("     'proto' is communication protocol used by the connection (UDP or TCP)\n");
   printf("\n");
   return;
}


/* end of source */
