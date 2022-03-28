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
#define _SRC_TRU_TINYRADUTIL_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "tinyradutil.h"

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


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

static const TinyRadUtilWidget *
tru_widget_lookup(
         const char *                     wname,
         int                              exact );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_cmdmap[]
const TinyRadUtilWidget tru_widget_map[] =
{
   {
      .name       = "configuration",
      .func       = &tru_widget_config,
      .shortopts  = TRU_COMMON_SHORT TRU_DICT_SHORT,
      .longopts   = (struct option []) { TRU_DICT_LONG TRU_COMMON_LONG },
      .min_arg    = 0,
      .max_arg    = 0,
      .usage      = NULL,
      .desc       = "print configuration"
   },
   {
      .name       = "dictionary",
      .func       = &tru_widget_dict,
      .shortopts  = TRU_COMMON_SHORT TRU_DICT_SHORT,
      .longopts   = (struct option []) { TRU_DICT_LONG TRU_COMMON_LONG },
      .min_arg    = 0,
      .max_arg    = 0,
      .usage      = NULL,
      .desc       = "print processed dictionary"
   },
   {
      .name       = "help",
      .func       = &tru_usage,
      .shortopts  = TRU_COMMON_SHORT,
      .longopts   = (struct option []) { TRU_COMMON_LONG },
      .min_arg    = 0,
      .max_arg    = 0,
      .usage      = NULL,
      .desc       = "print command usage"
   },
   {
      .name       = NULL,
      .func       = NULL,
      .shortopts  = NULL,
      .longopts   = NULL,
      .min_arg    = -1,
      .max_arg    = -1,
      .usage      = NULL,
      .desc       = NULL
   }
};


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
   TinyRadUtilConf    cnfdata;
   TinyRadUtilConf *  cnf;

   // getopt options
   static char          short_opt[] = "+" TRU_COMMON_SHORT;
   static struct option long_opt[]  = { TRU_COMMON_LONG };

   trutils_initialize(PROGRAM_NAME);

   memset(&cnfdata, 0, sizeof(cnfdata));
   cnf         = &cnfdata;
   dict        = NULL;

   while((c = tru_getopt(cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case TRU_GETOPT_MATCHED: /* captured by common options */
         case -1:                 /* no more arguments */
         case 0:                  /* long options toggles */
         break;

         case TRU_GETOPT_EXIT:
         tru_cleanup(cnf);
         return(0);

         case TRU_GETOPT_ERROR:
         tru_cleanup(cnf);
         return(1);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };
   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   cnf->cmd_argc = (argc - optind);
   cnf->cmd_argv = &argv[optind];
   cnf->cmd_name = cnf->cmd_argv[0];

   // looks up widget
   if ((cnf->cmd = tru_widget_lookup(cnf->cmd_argv[0], TRAD_NO)) == NULL)
   {
      fprintf(stderr, "%s: unknown or ambiguous widget -- \"%s\"\n", PROGRAM_NAME, cnf->cmd_argv[0]);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   // process widget cli options
   optind = 1;
   opt_index = 0;
   while((c = tru_getopt(cnf, cnf->cmd_argc, cnf->cmd_argv, cnf->cmd->shortopts, cnf->cmd->longopts, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case TRU_GETOPT_EXIT:
         tru_cleanup(cnf);
         return(0);

         case TRU_GETOPT_ERROR:
         tru_cleanup(cnf);
         return(1);

         case '?':
         fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->cmd_name);
         return(1);

         default:
         fprintf(stderr, "%s: %s: unrecognized option `--%c'\n", PROGRAM_NAME, cnf->cmd_name, c);
         fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->cmd_name);
         return(1);
      };
   };
   if ((cnf->cmd_argc - optind) < cnf->cmd->min_arg)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->cmd_name);
      return(1);
   };
   if ((optind+cnf->cmd->max_arg) > cnf->cmd_argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, cnf->cmd_argv[optind+1]);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->cmd_name);
      return(1);
   };

   // call widget
   rc = cnf->cmd->func(cnf);
   tru_cleanup(cnf);
   return(trutils_exit_code(rc));
}


void
tru_cleanup(
         TinyRadUtilConf *                 cnf )
{
   tinyrad_free(cnf->tr);
   tinyrad_strsfree(cnf->dict_files);
   tinyrad_strsfree(cnf->dict_paths);
   return;
}


int
tru_getopt(
         TinyRadUtilConf *                 cnf,
         int                           argc,
         char * const *                argv,
         const char *                  short_opt,
         const struct option *         long_opt,
         int *                         opt_index )
{
   int   c;
   int   opt;

   switch(c = getopt_long(argc, argv, short_opt, long_opt, opt_index))
   {
      case 'b':
      cnf->tr_opts |= TRAD_BUILTIN_DICT;
      return(TRU_GETOPT_MATCHED);

      case 'D':
      if (tinyrad_strsadd(&cnf->dict_files, optarg) != TRAD_SUCCESS)
      {
         trutils_error(cnf->opts, NULL, "out of virtual memory");
         return(TRU_GETOPT_ERROR);
      };
      return(TRU_GETOPT_MATCHED);

      case 'd':
      opt = ((optarg)) ? (int)strtol(optarg, NULL, 0) : TRAD_DEBUG_ANY;
      tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt);
      return(TRU_GETOPT_MATCHED);

      case 'h':
      tru_usage(cnf);
      return(TRU_GETOPT_EXIT);

      case 'I':
      if (tinyrad_strsadd(&cnf->dict_paths, optarg) != TRAD_SUCCESS)
      {
         trutils_error(cnf->opts, NULL, "out of virtual memory");
         return(TRU_GETOPT_ERROR);
      };
      return(TRU_GETOPT_MATCHED);

      case 'q':
      cnf->opts |=  TRUTILS_OPT_QUIET;
      cnf->opts &= ~TRUTILS_OPT_VERBOSE;
      return(TRU_GETOPT_MATCHED);

      case 'V':
      trutils_version();
      return(TRU_GETOPT_EXIT);

      case 'v':
      cnf->opts |=  TRUTILS_OPT_VERBOSE;
      cnf->opts &= ~TRUTILS_OPT_QUIET;
      return(TRU_GETOPT_MATCHED);

      default:
      break;
   };

   return(c);
}


int
tru_load_tinyrad(
         TinyRadUtilConf *                 cnf )
{
   int                  rc;
   char **              errs;
   size_t               pos;
   TinyRadDict *        dict;

   errs = NULL;
   dict = NULL;

   if ((cnf->dict_files))
   {
      // initialize dictionary
      if ((rc = tinyrad_dict_initialize(&dict, cnf->tr_opts)) != TRAD_SUCCESS)
      {
         trutils_error(cnf->opts, NULL, "out of virtual memory");
         return(rc);
      };

      // set paths
      if ((cnf->dict_paths))
      {
         if ((rc = tinyrad_dict_set_option(dict, TRAD_DICT_OPT_PATHS, cnf->dict_paths)) != TRAD_SUCCESS)
         {
            trutils_error(cnf->opts, NULL, "tinyrad_dict_set_option(TRAD_DICT_OPT_PATHS): %s", tinyrad_strerror(rc));
            tinyrad_free(dict);
            return(rc);
         };
      };

      // parse dictionary files
      for(pos = 0; ( ((cnf->dict_files)) && ((cnf->dict_files[pos])) ); pos++)
      {
         if ((rc = tinyrad_dict_parse(dict, cnf->dict_files[pos], &errs, 0)) != TRAD_SUCCESS)
         {
            trutils_error(cnf->opts, errs, NULL);
            tinyrad_strsfree(errs);
            tinyrad_free(dict);
            return(rc);
         };
      };
   };

   // initialize tinyrad handle
   rc = tinyrad_initialize(&cnf->tr, dict, cnf->url, cnf->tr_opts);
   tinyrad_free(dict);
   return(rc);
}


int
tru_usage(
         TinyRadUtilConf *                 cnf )
{
   int            i;
   const char *   name;
   const char *   help;
   const char *   s;

   name  = (!(cnf->cmd)) ? "widget"         : cnf->cmd->name;
   help  = (!(cnf->cmd)) ? " ..."           : (((cnf->cmd->usage))     ? cnf->cmd->usage     : "");
   s     = (!(cnf->cmd)) ? TRU_COMMON_SHORT : (((cnf->cmd->shortopts)) ? cnf->cmd->shortopts : TRU_COMMON_SHORT);

   printf("Usage: %s %s [OPTIONS]%s\n", PROGRAM_NAME, name, help);
   printf("OPTIONS:\n");
   if ((strchr(s, 'b'))) printf("  -b, --builtin-dict        load built-in dictionary\n");
   if ((strchr(s, 'D'))) printf("  -D dictionary             include dictionary\n");
   if ((strchr(s, 'd'))) printf("  -d level, --debug=level   print debug messages\n");
   if ((strchr(s, 'f'))) printf("  -f file, --file=file      attribute list\n");
   if ((strchr(s, 'h'))) printf("  -h, --help                print this help and exit\n");
   if ((strchr(s, 'I'))) printf("  -I path                   add path to dictionary search paths\n");
   if ((strchr(s, 'q'))) printf("  -q, --quiet, --silent     do not print messages\n");
   if ((strchr(s, 'V'))) printf("  -V, --version             print version number and exit\n");
   if ((strchr(s, 'v'))) printf("  -v, --verbose             print verbose messages\n");
   if (!(cnf->cmd))
   {
      printf("COMMANDS:\n");
      for(i = 0; tru_widget_map[i].name != NULL; i++)
         if ((tru_widget_map[i].desc))
            printf("  %-25s %s\n", tru_widget_map[i].name, tru_widget_map[i].desc);
   };
   printf("\n");
   return(0);
}


static const TinyRadUtilWidget *
tru_widget_lookup(
         const char *                     wname,
         int                              exact )
{
   size_t                     wname_len;
   size_t                     x;
   const TinyRadUtilWidget *  widget;
   const TinyRadUtilWidget *  match;

   match       = NULL;
   wname_len   = strlen(wname);

   for(x = 0; ((tru_widget_map[x].name)); x++)
   {
      widget = &tru_widget_map[x];
      if (widget->func == NULL)
         continue;

      if (!(strncmp(widget->name, wname, wname_len)))
      {
         if (widget->name[wname_len] == '\0')
            return(widget);
         if ((match))
            return(NULL);
         if (exact == TRAD_NO)
            match = widget;
      };
   };

   return(match);
}

/* end of source */
