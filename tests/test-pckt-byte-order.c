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
#define _TESTS_TINYRAD_PCKT_BYTE_ORDER_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "common.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "test-pckt-byte-order"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main(
         int                           argc,
         char *                        argv[] );


void
my_print_header(
         unsigned                      opts,
         size_t                        width );


int
my_test_value(
         unsigned                      opts,
         size_t                        width,
         size_t                        offset,
         uint64_t                      value );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int                  opt;
   int                  c;
   int                  opt_index;
   unsigned             opts;
   size_t               pos;
   uint64_t             value;
   int                  errs;

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

   opts = 0;
   errs = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'd':
         opt = TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL,  &opt);
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

   my_print_header(opts, sizeof(uint16_t));
   value = 0x2211;
   for(pos = 0; (pos < (sizeof(uint64_t)+1)); pos++)
      errs += my_test_value( opts, sizeof(uint16_t), pos, value );
   if ((opts & TRAD_TEST_VERBOSE)) printf("\n");

   my_print_header(opts, sizeof(uint32_t));
   value = 0x44332211;
   for(pos = 0; (pos < (sizeof(uint64_t)+1)); pos++)
      errs += my_test_value( opts, sizeof(uint32_t), pos, value );
   if ((opts & TRAD_TEST_VERBOSE)) printf("\n");

   my_print_header(opts, sizeof(uint64_t));
   value = 0x8877665544332211;
   for(pos = 0; (pos < (sizeof(uint64_t)+1)); pos++)
      errs += my_test_value( opts, sizeof(uint64_t), pos, value );
   if ((opts & TRAD_TEST_VERBOSE)) printf("\n");

   return( (((errs)) ? 1 : 0) );
}


void
my_print_header(
         unsigned                      opts,
         size_t                        width )
{
   const char *   htop_str;
   const char *   hton_str;
   const char *   ptoh_str;

   if (!(opts & TRAD_TEST_VERBOSE))
      return;

   switch(width)
   {
      case 2:
      hton_str = "htons(value)";
      htop_str = "tinyrad_htops(&b[x], value)";
      ptoh_str = "tinyrad_ptohs(&b[x])";
      break;

      case 4:
      hton_str = "htonl(value)";
      htop_str = "tinyrad_htopl(&b[x], value)";
      ptoh_str = "tinyrad_ptohl(&b[x])";
      break;

      case 8:
      hton_str = "tinyrad_htonll(value)"; // htonll() is not standard
      htop_str = "tinyrad_htopll(&b[x], value)";
      ptoh_str = "tinyrad_ptohll(&b[x])";
      break;

      default:
      hton_str = "hton(value)";
      htop_str = "tinyrad_htop(&b[x], value)";
      ptoh_str = "tinyrad_ptoh(&b[x])";
      break;
   };
   printf(
      "           x  %18s  %39s  %21s  %18s\n",
      "value",
      htop_str,
      ptoh_str,
      hton_str
   );
   return;
}


int
my_test_value(
         unsigned                      opts,
         size_t                        width,
         size_t                        offset,
         uint64_t                      value )
{
   size_t         pos;
   uint64_t       ptoh;
   uint64_t       hton;
   uint8_t        buff[sizeof(uint64_t)*2];
   uint8_t        byte;
   const char *   title;

   memset(buff, 0xff, sizeof(buff));

   switch(width)
   {
      case 2:
      title = "uint16_t:";
      hton  = htons(value);
              tinyrad_htops(&buff[offset], value);
      ptoh  = tinyrad_ptohs(&buff[offset]);
      break;

      case 4:
      title = "uint32_t:";
      hton  = htonl(value);
              tinyrad_htopl(&buff[offset], value);
      ptoh  = tinyrad_ptohl(&buff[offset]);
      break;

      case 8:
      title = "uint64_t:";
      hton  = tinyrad_htonll(value); // htonll() is not standard
              tinyrad_htopll(&buff[offset], value);
      ptoh  = tinyrad_ptohll(&buff[offset]);
      break;

      default:
      return(1);
   };

   if ((opts & TRAD_TEST_VERBOSE)) printf("%s", title);
   if ((opts & TRAD_TEST_VERBOSE)) printf("  %zu", offset);
   if ((opts & TRAD_TEST_VERBOSE)) printf("  0x%016" PRIx64, value);
   if ((opts & TRAD_TEST_VERBOSE)) printf(" ");
   if ((opts & TRAD_TEST_VERBOSE))
   {
      for(pos = 0; (pos < sizeof(buff)); pos++)
      {
         if (!(pos & 0x01))
            printf(" ");
         printf("%02" PRIx8, buff[pos]);
      };
   };
   if ((opts & TRAD_TEST_VERBOSE)) printf("     0x%016" PRIx64, ptoh);
   if ((opts & TRAD_TEST_VERBOSE)) printf("  0x%016" PRIx64, hton);
   if ((opts & TRAD_TEST_VERBOSE)) printf("\n");

   if ( value != ptoh)
   {
      our_error(opts, NULL, "value parsed from buffer does not match original");
      return(1);
   };

   for(pos = 0; (pos < sizeof(buff)); pos++)
   {
      if ( (pos >= offset) && (pos < (offset+width)) )
      {
         byte = ((offset+width-pos) << 4) | ((offset+width-pos) << 0);
         if (buff[pos] != byte)
         {
            our_error(opts, NULL, "unexpected byte in buffer at offset %zu", pos);
            return(1);
         };
      } else {
         if (buff[pos] != 0xff)
         {
            our_error(opts, NULL, "unexpected byte in buffer at offset %zu", pos);
            return(1);
         };
      };
   };

   return(0);
}


/* end of source */
