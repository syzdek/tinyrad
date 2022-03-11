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

#include <tinyrad_utils.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad"

#define MY_OPT_DICT_DEFAULTS  0x0001UL
#define MY_OPT_DICT_DUMP      0x0002UL
#define MY_OPT_DICT_LOADED    0x0004UL


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct my_request_av
{
   char * attr_name;
   char * attr_value;
} MyRequestAV;


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
my_dict_load(
         int                           opts,
         TinyRadDict **                dictp,
         char **                       dict_files,
         char **                       dict_paths );


void
my_usage( void );


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
   TinyRadDict *  dict;
   TinyRad *      tr;
   const char *   url;
   char **        dict_files;
   char **        dict_paths;
   unsigned       opts;

   // getopt options
   static char          short_opt[] = "D:d:f:hI:qVv";
   static struct option long_opt[] =
   {
      {"debug",            optional_argument, NULL, 'd' },
      {"file",             optional_argument, NULL, 'f' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"defaults",         no_argument,       NULL,  2  },
      {"dictionary-dump",  no_argument,       NULL,  1  },
      { NULL, 0, NULL, 0 }
   };

   trutils_initialize(PROGRAM_NAME);

   opts        = 0;
   dict        = NULL;
   dict_files  = NULL;
   dict_paths  = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         opts |= MY_OPT_DICT_DUMP;
         break;

         case 2:
         opts |= MY_OPT_DICT_DEFAULTS;
         break;

         case 'D':
         if (tinyrad_strsadd(&dict_files, optarg) != TRAD_SUCCESS)
         {
            trutils_error(opts, NULL, "out of virtual memory");
            tinyrad_strsfree(dict_files);
            tinyrad_strsfree(dict_paths);
            return(trutils_exit_code(TRAD_ENOMEM));
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
         if (tinyrad_strsadd(&dict_paths, optarg) != TRAD_SUCCESS)
         {
            trutils_error(opts, NULL, "out of virtual memory");
            tinyrad_strsfree(dict_files);
            tinyrad_strsfree(dict_paths);
            return(trutils_exit_code(TRAD_ENOMEM));
         };
         break;

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
   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      tinyrad_strsfree(dict_files);
      tinyrad_strsfree(dict_paths);
      return(1);
   };
   url = argv[optind];
   optind++;

   // load RADIUS dictionary
   if ((rc = my_dict_load(opts, &dict, dict_files, dict_paths)) != TRAD_SUCCESS)
   {
      tinyrad_strsfree(dict_files);
      tinyrad_strsfree(dict_paths);
      return(trutils_exit_code(rc));
   };
   tinyrad_strsfree(dict_paths);
   tinyrad_strsfree(dict_files);

   // initialize tinyrad handle
   if ((tinyrad_initialize(&tr, dict, url, 0)) != TRAD_SUCCESS)
   {
      tinyrad_free(dict);
      return(1);
   };
   tinyrad_free(dict);

   // display dictionary
   if ((opts & MY_OPT_DICT_DUMP))
   {
      tinyrad_get_option(tr, TRAD_OPT_DICTIONARY, &dict);
      tinyrad_dict_print(dict, 0xffff);
      tinyrad_free(tr);
      tinyrad_free(dict);
      return(0);
   };

   tinyrad_free(tr);

   return(0);
}


int
my_dict_load(
         int                           opts,
         TinyRadDict **                dictp,
         char **                       dict_files,
         char **                       dict_paths )
{
   int                  rc;
   char **              errs;
   size_t               pos;
   TinyRadDict *        dict;

   assert(dictp != NULL);

   errs = NULL;

   if (!(dict_files))
      return(TRAD_SUCCESS);

   // initialize dictionary
   if ((rc = tinyrad_dict_initialize(&dict, 0)) != TRAD_SUCCESS)
   {
      trutils_error(opts, NULL, "out of virtual memory");
      return(rc);
   };

   // load defaults
   if ((opts & MY_OPT_DICT_DEFAULTS))
   {
      if ((rc = tinyrad_dict_defaults(dict, &errs, 0)) != TRAD_SUCCESS)
      {
         trutils_error(opts, errs, NULL);
         tinyrad_strsfree(errs);
         tinyrad_free(dict);
         return(rc);
      };
   };

   // set paths
   for(pos = 0; ( ((dict_paths)) && ((dict_paths[pos])) ); pos++)
   {
      if ((rc = tinyrad_dict_add_path(dict, dict_paths[pos])) != TRAD_SUCCESS)
      {
         trutils_error(opts, NULL, "%s: %s", dict_paths[pos], tinyrad_strerror(rc));
         tinyrad_free(dict);
         return(rc);
      };
   };

   // parse dictionary files
   for(pos = 0; ( ((dict_files)) && ((dict_files[pos])) ); pos++)
   {
      if ((rc = tinyrad_dict_parse(dict, dict_files[pos], &errs, 0)) != TRAD_SUCCESS)
      {
         trutils_error(opts, errs, NULL);
         tinyrad_strsfree(errs);
         tinyrad_free(dict);
         return(rc);
      };
   };

   *dictp = dict;

   return(TRAD_SUCCESS);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] url\n", PROGRAM_NAME);
   printf("OPTIONS:\n");
   printf("  -D dictionary             include dictionary\n");
   printf("  -d level, --debug=level   print debug messages\n");
   printf("  -f file, --file=file      attribute list\n");
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


/* end of source */
