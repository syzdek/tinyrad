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
#define _TESTS_TINYRAD_STR_EXPAND_C 1


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
#define PROGRAM_NAME "tinyrad-str-expand"

#define TEST_OPT_FORCE     0x0040UL


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
#pragma mark - Datatypes

typedef struct my_test_strings
{
   const char *   origin;
   const char *   expand;
   intptr_t       force;
} MyTestStr;


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const MyTestStr  test_strings[] =
{
   { "asdfghj",                                  "asdfghj",       TRAD_NO },
   { "'asdfghj'",                                "asdfghj",       TRAD_NO },
   { "\"asdfghj\"",                              "asdfghj",       TRAD_NO },
   { "\"\\141\\163\\144\\146\\147\\150\\152\"",  "asdfghj",       TRAD_NO },
   { "\"\\x61\\x73\\x64\\x66\\x67\\x68\\x6a\"",  "asdfghj",       TRAD_NO },
   { "\"a\\163\\x64f\\147\\x68j\"",              "asdfghj",       TRAD_NO },
   { "\"a%%sdfghj\"",                            "a%sdfghj",      TRAD_NO },
   { "\"%u:x:%U:%G:test user:%d:/bin/false\"",   NULL,            TRAD_NO },
   { "\"%g:x:%G:%u,root\"",                      NULL,            TRAD_NO },
   { "\"Is %p running successfully?\"",          NULL,            TRAD_NO },
   { "\"Is your email address %u@%D\?\"",        NULL,            TRAD_NO },
   { "\"is your email address %u@%H\?\"",        NULL,            TRAD_NO },
   { "\"Is your email address %u@%h.%D\?\"",     NULL,            TRAD_NO },
   { NULL, NULL, 0 }
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
   int                           opt;
   int                           c;
   int                           opt_index;
   size_t                        pos;
   unsigned                      opts;
   char                          buff[TRAD_LINE_MAX_LEN];
   const MyTestStr *             test_str;

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
      {"force",            no_argument,       NULL, 1   },
      { NULL, 0, NULL, 0 }
   };

   trutils_initialize(PROGRAM_NAME);

   opts = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         opts |= TEST_OPT_FORCE;
         break;

         case 'd':
         opt = TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL,  &opt);
         break;

         case 'h':
         printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
         printf("       %s [OPTIONS] str1 [ str2 [ ... [ strN ] ] ]\n", PROGRAM_NAME);
         printf("OPTIONS:\n");
         printf("  -d, --debug               print debug messages\n");
         printf("  -h, --help                print this help and exit\n");
         printf("  -q, --quiet, --silent     do not print messages\n");
         printf("  -V, --version             print version number and exit\n");
         printf("  -v, --verbose             print verbose messages\n");
         printf("  --force                   force expansion of user strings\n");
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

   // expand custom strings
   if (optind < argc)
   {
      while (optind < argc)
      {
         trutils_verbose(opts, "test string:  %s", argv[optind]);
         if (!(tinyrad_strexpand(buff,  argv[optind], sizeof(buff), (((opts&TEST_OPT_FORCE)) ? TRAD_YES : TRAD_NO))))
         {
            trutils_error(opts, NULL, "tinyrad_strexpand(): error occurred");
            return(1);
         };
         buff[sizeof(buff)-1] = '\0';
         if (!(opts & TRUTILS_OPT_VERBOSE))
            trutils_print(opts, "%s", buff);
         trutils_verbose(opts, "         ==>  %s", buff);
         optind++;
         if (optind < argc)
            trutils_verbose(opts, "");
     };
      return(0);
   };

   // expand and verify test strings
   for(pos = 0; ((test_strings[pos].origin)); pos++)
   {
      test_str = &test_strings[pos];
      trutils_verbose(opts, "test string:  %s", test_strings[pos].origin);
      if (!(tinyrad_strexpand(buff, test_str->origin, sizeof(buff), (int)test_str->force)))
      {
         trutils_error(opts, NULL, "tinyrad_strexpand(): error occurred");
         return(1);
      };
      buff[sizeof(buff)-1] = '\0';
      trutils_verbose(opts, "         ==>  %s", buff);
      if ((test_strings[pos].expand))
      {
         if ((strcmp(buff, test_strings[pos].expand)))
         {
            trutils_error(opts, NULL, "expected: %s", test_strings[pos].expand);
            return(1);
         };
      };
      trutils_verbose(opts, "");
   };

   return(0);
}


/* end of source */

