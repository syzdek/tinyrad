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
#define _TESTS_TEST_ASSUMPTIONS_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#undef NDEBUG

#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <stdatomic.h>

#include <tinyrad.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "test-assumptions"


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
#pragma mark - Datatypes

struct my_data8
{
   char        member0;
   char        member1;
   char        member2;
   uint8_t     member3;
   uint8_t     member4;
   uint8_t     member5;
   int8_t      member6;
   int8_t      member7;
   int8_t      member8;
};

struct my_data16
{
   uint16_t    member0;
   uint16_t    member1;
   uint16_t    member2;
   int16_t     member3;
   int16_t     member4;
   int16_t     member5;
};

struct my_data32
{
   uint32_t    member0;
   uint32_t    member1;
   uint32_t    member2;
   int32_t     member3;
   int32_t     member4;
   int32_t     member5;
};

struct my_data64
{
   uint64_t    member0;
   uint64_t    member1;
   uint64_t    member2;
   int64_t     member3;
   int64_t     member4;
   int64_t     member5;
};

struct my_data_mixed
{
   char        member0;
   char        member1;
   char        member2;
   uint8_t     member3;
   uint8_t     member4;
   uint8_t     member5;
   int8_t      member6;
   int8_t      member7;

   uint16_t    member8;
   uint16_t    member9;
   int16_t     member10;
   int16_t     member11;

   uint32_t    member12;
   int32_t     member13;

   uint64_t    member14;

   int64_t     member15;

   int8_t      member16;
   int8_t      member17;
   int16_t     member18;
   int32_t     member19;
};


struct my_data_intptr
{
   intptr_t    member0;
   intptr_t    member1;
   intptr_t    member2;
   uintptr_t   member3;
   uintptr_t   member4;
   uintptr_t   member5;
};


struct my_data_ptr
{
   void *      member0;
   void *      member1;
   void *      member2;
   void *      member3;
   void *      member4;
   void *      member5;
};


struct my_obj
{
   uint8_t                 magic[8];
   atomic_intptr_t         ref_count;
   void (*free_func)(void * ptr);
};


struct my_data
{
   struct my_obj     obj;
   int8_t            member0;
   int8_t            member1;
   int16_t           member2;
   int32_t           member3;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
main(
         int                           argc,
         char *                        argv[] );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
main(
         int                           argc,
         char *                        argv[] )
{
   int                           c;
   int                           opt_index;
   unsigned                      opts;

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

   opts  = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'd':
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
         opts |=  TRUTILS_OPT_QUIET;
         opts &= ~TRUTILS_OPT_VERBOSE;
         break;

         case 'V':
         printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
         opts |=  TRUTILS_OPT_VERBOSE;
         opts &= ~TRUTILS_OPT_QUIET;
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

   // data type size assumptions
   assert(sizeof(  int8_t   )  ==  1 );
   assert(sizeof( uint8_t   )  ==  1 );
   assert(sizeof(  int16_t  )  ==  2 );
   assert(sizeof( uint16_t  )  ==  2 );
   assert(sizeof(  int32_t  )  ==  4 );
   assert(sizeof( uint32_t  )  ==  4 );
   assert(sizeof(  int64_t  )  ==  8 );
   assert(sizeof( uint64_t  )  ==  8 );

   // struct packing assumptions
   assert(sizeof( struct my_data8  )     ==  (1*9));
   assert(sizeof( struct my_data16 )     ==  (2*6));
   assert(sizeof( struct my_data32 )     ==  (4*6));
   assert(sizeof( struct my_data64 )     ==  (8*6));
   assert(sizeof( struct my_data_mixed ) ==  (8*6));
   assert(sizeof( struct my_data_ptr )   ==  sizeof( struct my_data_intptr) );
   assert(sizeof( struct my_data )       ==  (16 + (sizeof(void *) * 2)) );

   // pointer sizes
   assert(sizeof( void * )  >=  4);
   assert(sizeof( void * )  ==  sizeof(  intptr_t ));
   assert(sizeof( void * )  ==  sizeof( uintptr_t ));
   assert(sizeof( void * )  ==  sizeof(    size_t ));
   assert(sizeof( void * )  ==  sizeof(   ssize_t ));

   if ((opts & TRUTILS_OPT_VERBOSE))
      printf("%s: It appears that the developer's assumptions are correct.\n", PROGRAM_NAME);

   return(0);
}


/* end of source */

