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
#define _TESTS_TINYRAD_STR_SPLIT_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <tinyrad_utils.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include <tinyrad.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-str-split"


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const char * const * test_strings[] =
{
   (const char * const[]) { ":", "test", "string", "1", NULL },
   (const char * const[]) { "-", "test", "string", "1", NULL },
   (const char * const[]) { ",", "test", "string", "1", NULL },
   (const char * const[]) { "_", "test", "string", "1", NULL },
   (const char * const[]) { " ", "test", "string", "1", NULL },
   (const char * const[]) { ":", "test", "empty", "", "string", NULL },
   (const char * const[]) { ":", "test", NULL },
   (const char * const[]) { ":", "", "test", NULL },
   (const char * const[]) { ":", "test", "", NULL },
   (const char * const[]) { ":", "", NULL },
   (const char * const[]) { ":", "", "", NULL },
   (const char * const[]) { ":", "", "", "", NULL },
   (const char * const[]) { ":", "'This-is-a'", "'test:of'", "\"quoted,strings\"to\"split.\"", NULL },
   (const char * const[]) { "\"", "'This-is-a'", "'test:of'", "quoted,strings to split.", NULL },
   (const char * const[]) { "'", "\"This'is'a\"", "\"test:of\"", "\"quoted\\\"strings\\\"to\\\"split.\"", NULL },
   NULL
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


int
my_test_array(
         unsigned                      opts,
         const char * const *          argv,
         const char *                  delim );


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
   int            opt;
   int            c;
   int            opt_index;
   int            x;
   int            rc;
   unsigned       opts;
   const char *   delim;

   // getopt options
   static char          short_opt[] = "dD:hVvq";
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

   trutils_initialize(PROGRAM_NAME);

   opts   = 0;
   delim  = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'D':
         delim = optarg;
         break;

         case 'd':
         opt = TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL,  &opt);
         break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("       %s [OPTIONS] str1 [ str2 [ ... [ strN ] ] ]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -D character              use 'character' as deliminator\n");
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
         trutils_version();
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

   if (optind < argc)
   {
      delim = ((delim)) ? delim : ":";
      if ((rc = my_test_array(opts, (const char *const *)&argv[optind], delim)) != 0)
         return(1);
      return(0);
   };

   for(x = 0; ((test_strings[x])); x++)
      if ((rc = my_test_array(opts, &test_strings[x][1], test_strings[x][0])) != 0)
         return(1);

   return(0);
}


int
my_test_array(
         unsigned                      opts,
         const char * const *          argv,
         const char *                  delim )
{
   int         rc;
   int         len;
   int         pos;
   char        buff[1024];
   char **     strs;

   strs    = NULL;
   buff[0] = '\0';
   delim   = ((delim)) ? delim : ":";

   // build test string
   for(pos = 0; ((argv[pos])); pos++)
   {
      if ((pos))
         tinyrad_strlcat(buff, delim, sizeof(buff));
      tinyrad_strlcat(buff, argv[pos], sizeof(buff));
   };

   trutils_verbose(opts, "string(%c): \"%s\"", delim[0], buff);

   if ((rc = tinyrad_strsplit(buff, delim[0], &strs, &len)) != TRAD_SUCCESS)
   {
      trutils_error(opts, NULL, "tinyrad_strsplit(): %s", tinyrad_strerror(rc));
      return(1);
   };
   if (!(strs))
   {
      trutils_error(opts, NULL, "tinyrad_strsplit(): did not return data");
      return(1);
   };

   // print result
   for(pos = 0; ((strs[pos])); pos++)
      trutils_verbose(opts, "    arg %i: \"%s\"", pos, strs[pos]);

   // check result against original
   if (len != pos)
   {
      trutils_error(opts, NULL, "tinyrad_strsplit(): returned data length did not patch returned length");
      return(1);
   };
   for(pos = 0; ((argv[pos])); pos++);
   if (len != pos)
   {
      trutils_error(opts, NULL, "tinyrad_strsplit(): returned %i strings, expected %i strings", len, pos);
      return(1);
   };
   for(pos = 0; ((argv[pos])); pos++)
   {
      if ((strcmp(argv[pos], strs[pos])))
      {
         trutils_error(opts, NULL, "tinyrad_strsplit(): strings do not match originals ");
         return(1);
      };
   };

   tinyrad_free(strs);

   return(TRAD_SUCCESS);
}

/* end of source */

