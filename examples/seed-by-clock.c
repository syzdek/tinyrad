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
#define _SRC_SEED_BY_CLOCK_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

//#include <tinyrad_compat.h>

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "seed-by-clock"
#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT "unknown"
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT "unknown"
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "Tiny RADIUS Client Library"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION "unknown"
#endif

#undef _U
#define _U( num ) ((unsigned) num )


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
   int               c;
   int               opt_index;
   unsigned          rnd;
   unsigned          tmp;
   struct timespec   ts_rt1;
   struct timespec   ts_mono;
   struct timespec   ts_cpu;
   struct timespec   ts_comb;

   // getopt options
   static char          short_opt[] = "hqVv";
   static struct option long_opt[] =
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      { NULL, 0, NULL, 0 }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("Options:\n");
         printf("  -h, --help                                print this help and exit\n");
         printf("  -q, --quiet, --silent                     do not print messages\n");
         printf("  -V, --version                             print version number and exit\n");
         printf("  -v, --verbose                             print verbose messages\n");
         printf("\n");
         return(0);

         case 'q':
         break;

         case 'V':
         printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
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

   // read initial clocks
   clock_gettime(CLOCK_REALTIME,           &ts_rt1);
   clock_gettime(CLOCK_MONOTONIC,          &ts_mono);
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_cpu);

   // add seconds and nanoseconds
   ts_comb.tv_sec   = ts_rt1.tv_sec  + ts_mono.tv_sec  + ts_cpu.tv_sec;
   ts_comb.tv_nsec  = ts_rt1.tv_nsec + ts_mono.tv_nsec + ts_cpu.tv_nsec;
   rnd              = (unsigned)(ts_comb.tv_sec + ts_comb.tv_nsec);
   tmp              = rnd;

   // reverse bits
   rnd = ((rnd & 0xffff0000) >> 16) | ((rnd & 0x0000ffff) << 16);
   rnd = ((rnd & 0xff00ff00) >>  8) | ((rnd & 0x00ff00ff) <<  8);
   rnd = ((rnd & 0xf0f0f0f0) >>  4) | ((rnd & 0x0f0f0f0f) <<  4);
   rnd = ((rnd & 0xcccccccc) >>  2) | ((rnd & 0x33333333) <<  2);
   rnd = ((rnd & 0xaaaaaaaa) >>  1) | ((rnd & 0x55555555) <<  1);

   // print initial results
   printf("Round 1:\n");
   printf("   realtime:   %08x  %08x\n", (unsigned)ts_rt1.tv_sec,  (unsigned)ts_rt1.tv_nsec);
   printf("   monotonic:  %08x  %08x\n", (unsigned)ts_mono.tv_sec, (unsigned)ts_mono.tv_nsec);
   printf("   cputime:    %08x  %08x\n", (unsigned)ts_cpu.tv_sec,  (unsigned)ts_cpu.tv_nsec);
   printf("   combined:   %08x  %08x\n", (unsigned)ts_comb.tv_sec, (unsigned)ts_comb.tv_nsec);
   printf("   rnd:        %08x\n",       tmp);
   printf("   rnd (rev):  %08x\n",       rnd);

   // read clocks second time
   clock_gettime(CLOCK_REALTIME,           &ts_rt1);
   clock_gettime(CLOCK_MONOTONIC,          &ts_mono);
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_cpu);

   // add seconds and nanoseconds to initial result
   ts_comb.tv_sec   = ts_rt1.tv_sec  + ts_mono.tv_sec  + ts_cpu.tv_sec;
   ts_comb.tv_nsec  = ts_rt1.tv_nsec + ts_mono.tv_nsec + ts_cpu.tv_nsec;
   rnd             += (unsigned)(ts_comb.tv_sec + ts_comb.tv_nsec);

   // print final results
   printf("\n");
   printf("Round 2:\n");
   printf("   realtime:   %08x  %08x\n", (unsigned)ts_rt1.tv_sec,  (unsigned)ts_rt1.tv_nsec);
   printf("   monotonic:  %08x  %08x\n", (unsigned)ts_mono.tv_sec, (unsigned)ts_mono.tv_nsec);
   printf("   cputime:    %08x  %08x\n", (unsigned)ts_cpu.tv_sec,  (unsigned)ts_cpu.tv_nsec);
   printf("   combined:   %08x  %08x\n", (unsigned)ts_comb.tv_sec, (unsigned)ts_comb.tv_nsec);
   printf("   rnd:        %08x\n",       rnd);

   // seed random number generator
   srandom(rnd);

   // print random numbers
   printf("\n");
   printf("psuedo random numbers:\n");
   for(c = 0; (c < 14); c++)
   {
      printf(
         "     %08x  %08x  %08x  %08x  %08x  %08x  %08x\n",
         (unsigned)random(), (unsigned)random(),
         (unsigned)random(), (unsigned)random(),
         (unsigned)random(), (unsigned)random(),
         (unsigned)random()
      );
   };

   return(0);
}


/* end of source */
