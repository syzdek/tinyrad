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
#define _TESTS_TINYRAD_URL_RESOLVE_C 1


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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tinyrad.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#ifndef PROGRAM_NAME
#   define PROGRAM_NAME "tinyrad-url-resolve"
#endif
#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT ""
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME ""
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION ""
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main( int argc, char * argv[] );

int my_test_bad(const char * url, int verbose);
int my_test_good(const char * url, int verbose);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const char * test_url_good_strs[] =
{
   "radius://203.0.113.45/drowssap",
   "radius://203.0.113.45/drowssap?tcp",
   "radius://203.0.113.45/drowssap?udp",
   "radius://203.0.113.45:1111/drowssap",
   "radius://203.0.113.45:1111/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",

   "radius-acct://203.0.113.45/drowssap",
   "radius-acct://203.0.113.45/drowssap?tcp",
   "radius-acct://203.0.113.45/drowssap?udp",
   "radius-acct://203.0.113.45:1111/drowssap",
   "radius-acct://203.0.113.45:1111/drowssap?tcp",
   "radius-acct://203.0.113.45:1111/drowssap?udp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",

   "radius-dynauth://203.0.113.45/drowssap",
   "radius-dynauth://203.0.113.45/drowssap?tcp",
   "radius-dynauth://203.0.113.45/drowssap?udp",
   "radius-dynauth://203.0.113.45:1111/drowssap",
   "radius-dynauth://203.0.113.45:1111/drowssap?tcp",
   "radius-dynauth://203.0.113.45:1111/drowssap?udp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",

   "radsec://203.0.113.45",
   "radsec://203.0.113.45/",
   "radsec://203.0.113.45/?tcp",
   "radsec://203.0.113.45/?udp",
   "radsec://203.0.113.45:1111",
   "radsec://203.0.113.45:1111/",
   "radsec://203.0.113.45:1111/?tcp",
   "radsec://203.0.113.45:1111/?udp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/?tcp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/?udp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/?tcp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/?udp",

   NULL
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main( int argc, char * argv[] )
{
   int                  c;
   int                  opt_index;
   int                  verbose;
   size_t               pos;

   // getopt options
   static char          short_opt[] = "hVvq";
   static struct option long_opt[] =
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   verbose = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -h, --help                print this help and exit\n");
         printf("  -q, --quiet, --silent     do not print messages\n");
         printf("  -V, --version             print version number and exit\n");
         printf("  -v, --verbose             print verbose messages\n");
         printf("URL SYNTAX:\n");
         printf("   radius://hostport/secret[?proto]          (default proto: udp, port: 1812)\n");
         printf("   radius-acct://hostport/secret[?proto]     (default proto: udp, port: 1813)\n");
         printf("   radsec://hostport/[?proto]                (default proto: tcp, port: 2083)\n");
         printf("   radius-dynauth://hostport/secret[?proto]  (default proto: udp, port: 3799)\n");
         printf("\n");
         return(0);

         case 'q':
         break;

         case 'V':
         printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
         verbose = 1;
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

   if (optind < argc)
   {
      while (optind < argc)
      {
         if ((my_test_good(argv[optind], verbose)))
            return(1);
         optind++;
      };
      return(0);
   };

   for(pos = 0; ((test_url_good_strs[pos])); pos++)
      if ((my_test_good(test_url_good_strs[pos], verbose)))
         return(1);

   return(0);
}


int my_test_good(const char * url, int verbose)
{
   char *            str;
   TinyRadURLDesc *  trudp;

   if ((verbose))
      printf("testing good url: \"%s\" ...\n", url);

   if (tinyrad_urldesc_parse(url, &trudp) != TRAD_SUCCESS)
   {
      if ((verbose))
         printf(">>> syntax error\n");
      return(1);
   };

   if ((str = tinyrad_urldesc2str(trudp)) == NULL)
   {
      if ((verbose))
         printf(">>> error generating URL\n");
      tinyrad_urldesc_free(trudp);
      return(1);
   };
   tinyrad_free(str);

   if (tinyrad_urldesc_resolve(trudp, 0) != TRAD_SUCCESS)
   {
      if ((verbose))
         printf(">>> error resolving URL\n");
      tinyrad_urldesc_free(trudp);
      return(1);
   };

   tinyrad_urldesc_free(trudp);

   return(0);
}

/* end of source */
