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
#define _SRC_CLIENT_TINYRAD_C 1


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

#include "common.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#ifndef PROGRAM_NAME
#   define PROGRAM_NAME "numconvert"
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

void my_error( char ** errs, const char * fmt, ... );
void my_usage( void );
void my_version( void );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int            c;
   int            opt_index;
   int            rc;
   TinyRadDict *  dict;
   char **        errs;
   uint8_t        dictdump;
   uint8_t        dictloaded;

   // getopt options
   static char          short_opt[] = "D:hI:qVv";
   static struct option long_opt[] =
   {
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"defaults",         no_argument,       NULL, 2   },
      {"dictionary-dump",  no_argument,       NULL, 1   },
      { NULL, 0, NULL, 0 }
   };

   dictdump   = 0;
   dictloaded = 0;

   if (tinyrad_dict_initialize(&dict, 0) != TRAD_SUCCESS)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         dictdump++;
         break;

         case 2:
         if (tinyrad_dict_defaults(dict, &errs, 0) != TRAD_SUCCESS)
         {
            my_error(errs, NULL);
            tinyrad_dict_destroy(dict);
            return(1);
         };
         dictloaded++;
         break;

         case 'D':

         if (tinyrad_dict_import(dict, optarg, &errs, 0) != TRAD_SUCCESS)
         {
            tinyrad_dict_destroy(dict);
            my_error(errs, NULL);
            tinyrad_strings_free(errs);
            return(1);
         };
         dictloaded++;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'I':
         if ((rc = tinyrad_dict_add_path(dict, optarg)) != TRAD_SUCCESS)
         {
            tinyrad_dict_destroy(dict);
            my_error(NULL, "%s: %s", optarg, tinyrad_strerror(rc));
            return(1);
         };
         break;

         case 'V':
         my_version();
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

   if (!(dictloaded))
   {
      if (tinyrad_dict_defaults(dict, &errs, 0) != TRAD_SUCCESS)
      {
         my_error(errs, NULL);
         tinyrad_dict_destroy(dict);
         return(1);
      };
   };

   if ((dictdump))
   {
      tinyrad_dict_print(dict, 0xffff);
      tinyrad_dict_destroy(dict);
      return(0);
   };
printf("got here 3,000,000,000\n");

   tinyrad_dict_destroy(dict);
printf("got here 4,000,000,000\n");

   return(0);
}


void my_error( char ** errs, const char * fmt, ... )
{
   int     pos;
   va_list args;

   if ((errs))
      for(pos = 0; ((errs[pos])); pos++)
         fprintf(stderr, "%s: %s\n", PROGRAM_NAME, errs[pos]);

   if (!(fmt))
      return;

   fprintf(stderr, "%s: ", PROGRAM_NAME);
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   fprintf(stderr, "\n");

   return;
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("OPTIONS:\n");
   printf("  -D dictionary             include dictionary\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -I path                   add path to dictionary search paths\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  --defaults                load default dictionaries\n");
   printf("  --dictionary-dump         print imported dictionaries\n");
   printf("\n");
   return;
}


void my_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   return;
}


/* end of source */
