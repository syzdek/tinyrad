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
#define _SRC_UTILITIES_TINYRAD_C 1


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


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad"

#define MY_OPT_VERBOSE        0x0001
#define MY_OPT_BUILTIN        0x0002
#define MY_OPT_DUMP           0x0004


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
   int            opt;
   int            c;
   int            opt_index;
   int            rc;
   int            opts;
   TinyRadDict *  dict;
   size_t         pos;
   char **        errs;
   char **        paths;
   char **        files;

   // getopt options
   static char          short_opt[] = "d:D:hI:qVv";
   static struct option long_opt[] =
   {
      {"debug",            optional_argument, NULL, 'd' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"defaults",         no_argument,       NULL,  2  },
      {"dump",             no_argument,       NULL,  1  },
      { NULL, 0, NULL, 0 }
   };

   opt = TRAD_OFF;
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt);
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_IDENT, PROGRAM_NAME);

   opts     = 0;
   paths    = NULL;
   files    = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         opts |= MY_OPT_DUMP;
         break;

         case 2:
         opts |= MY_OPT_BUILTIN;
         break;

         case 'D':
         if (our_strings_append(&files, optarg) != TRAD_SUCCESS)
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            our_strings_free(files);
            our_strings_free(paths);
            return(our_exit_code(TRAD_ENOMEM));
         };
         break;

         case 'd':
         opt = ((optarg)) ? (int)strtol(optarg, NULL, 0) : TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt);
         break;

         case 'h':
         my_usage();
         return(0);

         case 'I':
         if (our_strings_append(&paths, optarg) != TRAD_SUCCESS)
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            our_strings_free(files);
            our_strings_free(paths);
            return(our_exit_code(TRAD_ENOMEM));
         };
         break;

         case 'V':
         our_version(PROGRAM_NAME);
         return(0);

         case 'v':
         opts |= MY_OPT_VERBOSE;
         break;

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(our_exit_code(TRAD_EUNKNOWN));

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(our_exit_code(TRAD_EUNKNOWN));
      };
   };

   // initialize dictionary
   if ((rc = tinyrad_dict_initialize(&dict, 0)) != TRAD_SUCCESS)
   {
      our_strings_free(files);
      our_strings_free(paths);
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(our_exit_code(rc));
   };

   // load builtin/default dictionary objects
   if ((opts & MY_OPT_BUILTIN))
   {
      if ((rc = tinyrad_dict_defaults(dict, &errs, 0)) != TRAD_SUCCESS)
      {
         our_error(PROGRAM_NAME, errs, NULL);
         our_strings_free(files);
         our_strings_free(paths);
         tinyrad_dict_destroy(dict);
         return(our_exit_code(rc));
      };
   };

   // add paths
   for(pos = 0; ( ((paths)) && ((paths[pos])) ); pos++)
   {
      if ((rc = tinyrad_dict_add_path(dict, paths[pos])) != TRAD_SUCCESS)
      {
         our_strings_free(paths);
         our_strings_free(files);
         tinyrad_dict_destroy(dict);
         our_error(PROGRAM_NAME, NULL, "%s: %s", optarg, tinyrad_strerror(rc));
         return(our_exit_code(rc));
      };
   };

   // add dictionary files
   for(pos = 0; ( ((files)) && ((files[pos])) ); pos++)
   {
      if ((rc = tinyrad_dict_parse(dict, files[pos], &errs, 0)) != TRAD_SUCCESS)
      {
         our_error(PROGRAM_NAME, errs, NULL);
         tinyrad_strings_free(errs);
         our_strings_free(paths);
         our_strings_free(files);
         tinyrad_dict_destroy(dict);
         return(our_exit_code(rc));
      };
   };

   // dump dictionary
   if ((opts & MY_OPT_DUMP))
      tinyrad_dict_print(dict, 0xffff);

   // frees resources
   our_strings_free(paths);
   our_strings_free(files);
   tinyrad_dict_destroy(dict);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("OPTIONS:\n");
   printf("  -D dictionary             include dictionary\n");
   printf("  -d level, --debug=level   print debug messages\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -I path                   add path to dictionary search paths\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  --defaults                load default dictionaries\n");
   printf("  --dump                    print imported dictionaries\n");
   printf("\n");
   return;
}


/* end of source */
