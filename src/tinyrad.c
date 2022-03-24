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
#define MY_OPT_CONFIG_PRINT   0x0008UL


#define TINYRAD_GETOPT_SHORT "h"
#define TINYRAD_GETOPT_LONG \
   { "debug",            optional_argument, NULL, 'd' }, \
   { "file",             optional_argument, NULL, 'f' }, \
   { "help",             no_argument,       NULL, 'h' }, \
   { "quiet",            no_argument,       NULL, 'q' }, \
   { "silent",           no_argument,       NULL, 'q' }, \
   { "version",          no_argument,       NULL, 'V' }, \
   { "verbose",          no_argument,       NULL, 'v' }, \
   { "configuration",    no_argument,       NULL,  3  }, \
   { "defaults",         no_argument,       NULL,  2  }, \
   { "dictionary-dump",  no_argument,       NULL,  1  }, \
   { NULL, 0, NULL, 0 }


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct tinyrad_config       TinyRadConf;
typedef struct tinyrad_command      TinyRadCommand;


typedef struct my_request_av
{
   char * attr_name;
   char * attr_value;
} MyRequestAV;


struct tinyrad_config
{
   unsigned                opts;
   unsigned                tr_opts;
   TinyRadCommand *        cmd;
   TinyRad *               tr;
   const char *            url;
   char **                 dict_files;
   char **                 dict_paths;
};


struct tinyrad_command
{
   const char *            cmd_name;
   int  (*cmd_func)(TinyRadConf * cnf);
   const char *            cmd_shortopts;
   struct option *         cmd_longopts;
   int                     cmd_min_arg;
   int                     cmd_max_arg;
   const char *            cmd_help;
   const char *            cmd_desc;
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
tinyrad_load_dict(
         int                           opts,
         unsigned                      tr_opts,
         TinyRad **                    trp,
         const char *                  url,
         char **                       dict_files,
         char **                       dict_paths );


int
tinyrad_usage(
         TinyRadConf *                 cnf );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const TinyRadCommand tinyrad_cmdmap[] =
{
   {
      "help",                                         // command name
      &tinyrad_usage,                                 // entry function
      TINYRAD_GETOPT_SHORT,                           // getopt short options
      (struct option []) { TINYRAD_GETOPT_LONG },     // getopt long options
      0, 0,                                           // min/max arguments
      NULL,                                           // extra cli usage
      "print command usage"                           // command description
   },
   { NULL, NULL, NULL, NULL, -1, -1, NULL, NULL }
};

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
   TinyRadConf    cnfdata;
   TinyRadConf *  cnf;

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
      {"configuration",    no_argument,       NULL,  3  },
      {"defaults",         no_argument,       NULL,  2  },
      {"dictionary-dump",  no_argument,       NULL,  1  },
      { NULL, 0, NULL, 0 }
   };

   trutils_initialize(PROGRAM_NAME);

   memset(&cnfdata, 0, sizeof(cnfdata));
   cnf         = &cnfdata;
   dict        = NULL;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         cnf->opts |= MY_OPT_DICT_DUMP;
         break;

         case 2:
         cnf->tr_opts |= TRAD_BUILTIN_DICT;
         break;

         case 3:
         cnf->opts |= MY_OPT_CONFIG_PRINT;
         break;

         case 'D':
         if (tinyrad_strsadd(&cnf->dict_files, optarg) != TRAD_SUCCESS)
         {
            trutils_error(cnf->opts, NULL, "out of virtual memory");
            tinyrad_strsfree(cnf->dict_files);
            tinyrad_strsfree(cnf->dict_paths);
            return(trutils_exit_code(TRAD_ENOMEM));
         };
         break;

         case 'd':
         opt = ((optarg)) ? (int)strtol(optarg, NULL, 0) : TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt);
         break;

         case 'h':
         tinyrad_usage(cnf);
         return(0);

         case 'I':
         if (tinyrad_strsadd(&cnf->dict_paths, optarg) != TRAD_SUCCESS)
         {
            trutils_error(cnf->opts, NULL, "out of virtual memory");
            tinyrad_strsfree(cnf->dict_files);
            tinyrad_strsfree(cnf->dict_paths);
            return(trutils_exit_code(TRAD_ENOMEM));
         };
         break;

         case 'q':
         cnf->opts |=  TRUTILS_OPT_QUIET;
         cnf->opts &= ~TRUTILS_OPT_VERBOSE;
         break;

         case 'V':
         trutils_version();
         return(0);

         case 'v':
         cnf->opts |=  TRUTILS_OPT_VERBOSE;
         cnf->opts &= ~TRUTILS_OPT_QUIET;
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
      cnf->url = argv[optind];
      optind++;
   };
   if (optind < argc)
   {
      fprintf(stderr, "%s: unrecognized argument -- %s\n", PROGRAM_NAME, argv[optind]);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      tinyrad_strsfree(cnf->dict_files);
      tinyrad_strsfree(cnf->dict_paths);
      return(1);
   };

   // load TinyRad handle
   rc = tinyrad_load_dict(cnf->opts, cnf->tr_opts, &tr, cnf->url, cnf->dict_files, cnf->dict_paths);
   tinyrad_strsfree(cnf->dict_files);
   tinyrad_strsfree(cnf->dict_paths);
   if (rc != TRAD_SUCCESS)
      return(trutils_exit_code(rc));

   // display dictionary
   if ((cnf->opts & MY_OPT_DICT_DUMP))
   {
      tinyrad_get_option(tr, TRAD_OPT_DICTIONARY, &dict);
      tinyrad_dict_print(dict, 0xffff);
      tinyrad_free(tr);
      tinyrad_free(dict);
      return(0);
   };

   // display configuration
   if ((cnf->opts & MY_OPT_CONFIG_PRINT))
   {
      tinyrad_get_option(tr, TRAD_OPT_DICTIONARY, &dict);
      tinyrad_conf_print(tr, dict);
      tinyrad_free(tr);
      tinyrad_free(dict);
      return(0);
   };

   tinyrad_free(tr);

   return(0);
}


int
tinyrad_load_dict(
         int                           opts,
         unsigned                      tr_opts,
         TinyRad **                    trp,
         const char *                  url,
         char **                       dict_files,
         char **                       dict_paths )
{
   int                  rc;
   char **              errs;
   size_t               pos;
   TinyRadDict *        dict;

   assert(trp != NULL);

   errs = NULL;
   dict = NULL;
   *trp = NULL;

   if ((dict_files))
   {
      // initialize dictionary
      if ((rc = tinyrad_dict_initialize(&dict, tr_opts)) != TRAD_SUCCESS)
      {
         trutils_error(opts, NULL, "out of virtual memory");
         return(rc);
      };

      // set paths
      if ((dict_paths))
      {
         if ((rc = tinyrad_dict_set_option(dict, TRAD_DICT_OPT_PATHS, dict_paths)) != TRAD_SUCCESS)
         {
            trutils_error(opts, NULL, "tinyrad_dict_set_option(TRAD_DICT_OPT_PATHS): %s", tinyrad_strerror(rc));
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
   };

   // initialize tinyrad handle
   rc = tinyrad_initialize(trp, dict, url, tr_opts);
   tinyrad_free(dict);
   return(rc);
}


int
tinyrad_usage(
         TinyRadConf *                 cnf )
{
   int i;
   printf("Usage: %s [OPTIONS] [url]\n", PROGRAM_NAME);
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
   printf("  --configuration           print configuration\n");
   if (!(cnf->cmd))
   {
      printf("COMMANDS:\n");
      for(i = 0; tinyrad_cmdmap[i].cmd_name != NULL; i++)
         if ((tinyrad_cmdmap[i].cmd_desc))
            printf("  %-25s %s\n", tinyrad_cmdmap[i].cmd_name, tinyrad_cmdmap[i].cmd_desc);
   };
   printf("\n");
   return(0);
}


/* end of source */
