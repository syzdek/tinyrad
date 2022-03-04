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
#define _TESTS_TINYRAD_OPTIONS_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>

#include <tinyrad.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-options"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main( int argc, char * argv[] );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main( int argc, char * argv[] )
{
   int                  opt;
   int                  c;
   int                  opt_index;
   int                  rc;
   int                  debug;
   unsigned             opts;
   char *               str;
   TinyRad *            tr;
   struct timeval       tv;

   // getopt options
   static char          short_opt[] = "dhVvq";
   static struct option long_opt[] =
   {
      {"debug",            no_argument,       NULL, 'd' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   our_initialize(PROGRAM_NAME);

   debug = 0;
   opts  = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'd':
         debug = TRAD_DEBUG_ANY;
         break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -d, --debug               print debug messages\n");
         printf("  -h, --help                print this help and exit\n");
         printf("  -q, --quiet, --silent     do not print messages\n");
         printf("  -V, --version             print version number and exit\n");
         printf("  -v, --verbose             print verbose messages\n");
         printf("\n");
         return(0);

         case 'q':
         opts |=  TRAD_TEST_QUIET;
         opts &= ~TRAD_TEST_VERBOSE;
         break;

         case 'V':
         our_version(PROGRAM_NAME);
         return(0);

         case 'v':
         opts |=  TRAD_TEST_VERBOSE;
         opts &= ~TRAD_TEST_QUIET;
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

   // TRAD_OPT_DEBUG_SYSLOG
   opt = TRAD_ON;
   if ((rc = tinyrad_set_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DEBUG_SYSLOG, TRAD_ON): %s", tinyrad_strerror(rc)));
   opt = ~TRAD_ON;
   if ((rc = tinyrad_get_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DEBUG_SYSLOG, &opt): %s", tinyrad_strerror(rc)));
   if (opt != TRAD_ON)
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_SYSLOG does not match"));
   opt = TRAD_OFF;
   if ((rc = tinyrad_set_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DEBUG_SYSLOG, TRAD_ON): %s", tinyrad_strerror(rc)));
   opt = ~TRAD_OFF;
   if ((rc = tinyrad_get_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DEBUG_SYSLOG, &opt): %s", tinyrad_strerror(rc)));
   if (opt != TRAD_OFF)
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_SYSLOG does not match"));

   // TRAD_OPT_DEBUG_IDENT
   if ((rc = tinyrad_set_option(NULL, TRAD_OPT_DEBUG_IDENT, PROGRAM_NAME)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DEBUG_IDENT, PROGRAM_NAME): %s", tinyrad_strerror(rc)));
   str = NULL;
   if ((rc = tinyrad_get_option(NULL, TRAD_OPT_DEBUG_IDENT, &str)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DEBUG_IDENT, &str): %s", tinyrad_strerror(rc)));
   if (!(str))
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_IDENT is NULL"));
   if ((strcmp(PROGRAM_NAME, str)))
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_IDENT does not match"));
   free(str);

   // TRAD_OPT_DEBUG_LEVEL
   opt = TRAD_DEBUG_PACKETS;
   if ((rc = tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DEBUG_LEVEL, TRAD_DEBUG_PACKETS): %s", tinyrad_strerror(rc)));
   opt = ~TRAD_DEBUG_PACKETS;
   if ((rc = tinyrad_get_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DEBUG_LEVEL, &opt): %s", tinyrad_strerror(rc)));
   if (opt != TRAD_DEBUG_PACKETS)
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_LEVEL does not match"));
   opt = TRAD_DEBUG_NONE;
   if ((rc = tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DEBUG_LEVEL, TRAD_DEBUG_NONE): %s", tinyrad_strerror(rc)));
   opt = ~TRAD_DEBUG_NONE;
   if ((rc = tinyrad_get_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DEBUG_LEVEL, &opt): %s", tinyrad_strerror(rc)));
   if (opt != TRAD_DEBUG_NONE)
      return(our_error(opts, NULL, "value for TRAD_OPT_DEBUG_LEVEL does not match"));

   // enable debug
   if ((debug))
      tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL,  &debug);

   // initialize tinyrad
   if ((rc = tinyrad_initialize(&tr, NULL, "radius://localhost/secret", 0)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_initialize(tr, \"radius://localhost/secret\", 0): %s", tinyrad_strerror(rc)));

   // TRAD_OPT_DESC
   opt = 53;
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_DESC, &opt)) == TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_DESC, 53): was able to set file descriptor", tinyrad_strerror(rc)));
   opt = ~0x00;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_DESC, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_DESC, &opt): %s", tinyrad_strerror(rc)));

   // TRAD_OPT_IPV4
   //opt = TRAD_ON;
   //if ((rc = tinyrad_set_option(tr, TRAD_OPT_IPV4, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_IPV4, TRAD_ON): %s", tinyrad_strerror(rc)));
   //opt = (int)~0x00;
   //if ((rc = tinyrad_get_option(tr, TRAD_OPT_IPV4, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_IPV4, &opt): %s", tinyrad_strerror(rc)));
   //if (opt != TRAD_ON)
   //   return(our_error(opts, NULL, "value for TRAD_OPT_IPV4 does not match"));
   //opt = TRAD_OFF;
   //if ((rc = tinyrad_set_option(tr, TRAD_OPT_IPV4, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_IPV4, TRAD_ON): %s", tinyrad_strerror(rc)));
   //opt = ~TRAD_OFF;
   //if ((rc = tinyrad_get_option(tr, TRAD_OPT_IPV4, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_IPV4, &opt): %s", tinyrad_strerror(rc)));
   //if (opt != TRAD_OFF)
   //   return(our_error(opts, NULL, "value for TRAD_OPT_IPV4 does not match"));

   // TRAD_OPT_IPV6
   //opt = TRAD_ON;
   //if ((rc = tinyrad_set_option(tr, TRAD_OPT_IPV6, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_IPV6, TRAD_ON): %s", tinyrad_strerror(rc)));
   //opt = (int)~0x00;
   //if ((rc = tinyrad_get_option(tr, TRAD_OPT_IPV6, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_IPV6, &opt): %s", tinyrad_strerror(rc)));
   //if (opt != TRAD_ON)
   //   return(our_error(opts, NULL, "value for TRAD_OPT_IPV6 does not match"));
   //opt = TRAD_OFF;
   //if ((rc = tinyrad_set_option(tr, TRAD_OPT_IPV6, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_IPV6, TRAD_ON): %s", tinyrad_strerror(rc)));
   //opt = ~TRAD_OFF;
   //if ((rc = tinyrad_get_option(tr, TRAD_OPT_IPV6, &opt)) != TRAD_SUCCESS)
   //   return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_IPV6, &opt): %s", tinyrad_strerror(rc)));
   //if (opt != TRAD_OFF)
   //   return(our_error(opts, NULL, "value for TRAD_OPT_IPV6 does not match"));

   // TRAD_OPT_NETWORK_TIMEOUT
   tv.tv_sec  = 200;
   tv.tv_usec = 111;
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_NETWORK_TIMEOUT, &tv)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_NETWORK_TIMEOUT, &tv): %s", tinyrad_strerror(rc)));
   tv.tv_sec  = ~0x00;
   tv.tv_usec = ~0x00;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_NETWORK_TIMEOUT, &tv)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_NETWORK_TIMEOUT, &tv): %s", tinyrad_strerror(rc)));
   if (tv.tv_sec != 200)
      return(our_error(opts, NULL, "tv_sec for TRAD_OPT_NETWORK_TIMEOUT does not match"));
   if (tv.tv_usec != 111)
      return(our_error(opts, NULL, "tv_usec for TRAD_OPT_NETWORK_TIMEOUT does not match"));

   // TRAD_OPT_TIMEOUT
   opt = 53;
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, TRAD_ON): %s", tinyrad_strerror(rc)));
   opt = ~0x00;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_TIMEOUT, &opt)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_TIMEOUT, &opt): %s", tinyrad_strerror(rc)));
   if (opt != 53)
      return(our_error(opts, NULL, "value for TRAD_OPT_TIMEOUT does not match"));

   // TRAD_OPT_SOCKET_BIND_ADDRESSES
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, "127.0.0.1")) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, \"127.0.0.1\"): %s", tinyrad_strerror(rc)));
   str = NULL;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str): %s", tinyrad_strerror(rc)));
   if (!(str))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES is NULL"));
   if ((strcmp(str, "127.0.0.1 ::")))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES does not match"));
   free(str);
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, "::1")) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, \"127.0.0.1\"): %s", tinyrad_strerror(rc)));
   str = NULL;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str): %s", tinyrad_strerror(rc)));
   if (!(str))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES is NULL"));
   if ((strcmp(str, "0.0.0.0 ::1")))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES does not match"));
   free(str);
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, "::1 127.0.0.1")) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, \"127.0.0.1\"): %s", tinyrad_strerror(rc)));
   str = NULL;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str): %s", tinyrad_strerror(rc)));
   if (!(str))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES is NULL"));
   if ((strcmp(str, "127.0.0.1 ::1")))
      return(our_error(opts, NULL, "value for TRAD_OPT_SOCKET_BIND_ADDRESSES does not match"));
   free(str);

   // TRAD_OPT_URI
   if ((rc = tinyrad_set_option(tr, TRAD_OPT_URI, "radius://localhost/radiussecret")) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_set_option(tr, TRAD_OPT_URI, \"radius://localhost/radiussecret\"): %s", tinyrad_strerror(rc)));
   str = NULL;
   if ((rc = tinyrad_get_option(tr, TRAD_OPT_URI, &str)) != TRAD_SUCCESS)
      return(our_error(opts, NULL, "tinyrad_get_option(tr, TRAD_OPT_URI, &str): %s", tinyrad_strerror(rc)));
   if (!(str))
      return(our_error(opts, NULL, "value for TRAD_OPT_URI is NULL"));
   if ((strcmp(str, "radius://localhost/radiussecret")))
      return(our_error(opts, NULL, "value for TRAD_OPT_URI does not match"));
   free(str);

   return(0);
}


/* end of source */

