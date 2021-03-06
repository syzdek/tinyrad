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

int
main(
         int                           argc,
         char *                        argv[] );


const char *
tru_basename(
         const char *                     path );


void
tru_cleanup(
         TinyRadUtilConf *             cnf );


static const TinyRadUtilWidget *
tru_widget_lookup(
         const char *                  wname,
         int                           exact );


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
      .name       = "client",
      .desc       = "RADIUS client",
      .usage      = " reqtype [ attr=val attr=val ... attr=val ]",
      .aliases    = NULL,
      .func       = &tru_widget_client
   },
   {
      .name       = "config",
      .desc       = "print configuration",
      .usage      = NULL,
      .aliases    = (const char * const[]) { "configuration", NULL },
      .func       = &tru_widget_config
   },
   {
      .name       = "dict",
      .desc       = "print processed dictionary",
      .usage      = NULL,
      .aliases    = (const char * const[]) { "dictionary", NULL },
      .func       = &tru_widget_dict
   },
   {
      .name       = "url",
      .desc       = "process TinyRad URL",
      .usage      = " [ url url ... url ]",
      .aliases    = NULL,
      .func       = &tru_widget_url
   },
   {
      .name       = NULL,
      .func       = NULL,
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

//---------------//
// main function //
//---------------//
#pragma mark main function

int main(int argc, char * argv[])
{
   int                  rc;
   TinyRadDict *        dict;
   TinyRadUtilConf      cnfdata;
   TinyRadUtilConf *    cnf;
   char                 prog_name[128];

   // getopt options
   static char          short_opt[] = "+" TRU_COMMON_SHORT;
   static struct option long_opt[]  = { TRU_COMMON_LONG };

   trutils_initialize(PROGRAM_NAME);

   memset(&cnfdata, 0, sizeof(cnfdata));
   cnf            = &cnfdata;
   dict           = NULL;
   cnf->prog_name = tru_basename(argv[0]);

   // skip argument processing if called via alias
   if ((cnf->widget = tru_widget_lookup(cnf->prog_name, TRAD_YES)) != NULL)
   {
      cnf->argc        = argc;
      cnf->argv        = argv;
      cnf->widget_name = cnf->prog_name;
   };

   if (!(cnf->widget))
   {
      // process common cli options
      if ((rc = tru_cli_parse(cnf, argc, argv, short_opt, long_opt, &tru_usage)) != 0)
      {
         tru_cleanup(cnf);
         return((rc == TRU_GETOPT_EXIT) ? 0 : 1);
      };
      if ((argc - optind) < 1)
      {
         fprintf(stderr, "%s: missing required argument\n", cnf->prog_name);
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(1);
      };

      cnf->widget_name = argv[optind];
      cnf->argc        = (argc - optind);
      cnf->argv        = &argv[optind];

      // looks up widget
      if ((cnf->widget = tru_widget_lookup(argv[optind], TRAD_NO)) == NULL)
      {
         fprintf(stderr, "%s: unknown or ambiguous widget -- \"%s\"\n", cnf->prog_name, cnf->argv[0]);
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(1);
      };
      snprintf(prog_name, sizeof(prog_name), "%s %s", PROGRAM_NAME, cnf->widget->name);
      cnf->prog_name = prog_name;
   };

   // call widget
   rc = cnf->widget->func(cnf);
   tru_cleanup(cnf);
   return(trutils_exit_code(rc));
}


//-------------------//
// logging functions //
//-------------------//
#pragma mark logging functions

int
tru_error(
         TinyRadUtilConf *                cnf,
         int                              rc,
         const char *                     fmt,
         ... )
{
   va_list        args;
   const char *   prog_name;

   if ((cnf->opts & TRUTILS_OPT_QUIET))
      return(rc);

   prog_name = ((cnf->prog_name)) ? cnf->prog_name : PROGRAM_NAME;

   fprintf(stderr, "%s: ", prog_name);

   if (!(fmt))
   {
      fprintf(stderr, "%s\n", tinyrad_strerror(rc));
      return(rc);
   }

   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   fprintf(stderr, "\n");

   return(rc);
}


int
tru_errors(
         TinyRadUtilConf *                cnf,
         int                              rc,
         char **                          errs )
{
   int            pos;
   const char *   prog_name;

   if ((cnf->opts & TRUTILS_OPT_QUIET))
      return(rc);

   prog_name = ((cnf->prog_name)) ? cnf->prog_name : PROGRAM_NAME;

   if ((errs))
      for(pos = 0; ((errs[pos])); pos++)
         fprintf(stderr, "%s: %s\n", prog_name, errs[pos]);

   return(rc);
}


int
tru_printf(
         TinyRadUtilConf *                cnf,
         const char *                     fmt,
         ... )
{
   int            rc;
   va_list        args;
   if ((cnf->opts & TRUTILS_OPT_QUIET))
      return(0);
   va_start(args, fmt);
   rc = vprintf(fmt, args);
   va_end(args);
   return(rc);
}


int
tru_verbose(
         TinyRadUtilConf *                cnf,
         const char *                     fmt,
         ... )
{
   int            rc;
   va_list        args;
   if (!(cnf->opts & TRUTILS_OPT_VERBOSE))
      return(0);
   va_start(args, fmt);
   rc = vprintf(fmt, args);
   va_end(args);
   return(rc);
}


//-------------------------//
// miscellaneous functions //
//-------------------------//
#pragma mark miscellaneous functions

const char *
tru_basename(
         const char *                     path )
{
   const char * ptr;
   assert(path != NULL);
   if ((ptr = strrchr(path, '/')))
      return(&ptr[1]);
   return(path);
}


void
tru_cleanup(
         TinyRadUtilConf *                cnf )
{
   tinyrad_free(cnf->tr);
   tinyrad_strsfree(cnf->dict_files);
   tinyrad_strsfree(cnf->dict_paths);
   return;
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
         return(tru_error(cnf, rc, "out of virtual memory"));

      // set paths
      if ((cnf->dict_paths))
      {
         if ((rc = tinyrad_dict_set_option(dict, TRAD_DICT_OPT_PATHS, cnf->dict_paths)) != TRAD_SUCCESS)
         {
            tru_error(cnf, rc, "tinyrad_dict_set_option(TRAD_DICT_OPT_PATHS): %s", tinyrad_strerror(rc));
            tinyrad_free(dict);
            return(rc);
         };
      };

      // parse dictionary files
      for(pos = 0; ( ((cnf->dict_files)) && ((cnf->dict_files[pos])) ); pos++)
      {
         if ((rc = tinyrad_dict_parse(dict, cnf->dict_files[pos], &errs, 0)) != TRAD_SUCCESS)
         {
            tru_errors(cnf, rc, errs);
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


//-----------------//
// usage functions //
//-----------------//
#pragma mark usage functions

int
tru_cli_parse(
         TinyRadUtilConf *             cnf,
         int                           argc,
         char * const *                argv,
         const char *                  short_opt,
         const struct option *         long_opt,
         int (*usage_func)(TinyRadUtilConf * cnf, const char * short_opt ) )
{
   int            c;
   int            opt_index;

   optind    = 1;
   opt_index = 0;

   while((c = tru_getopt(cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case TRU_GETOPT_MATCHED: /* captured by common options */
         case -1:                 /* no more arguments */
         case 0:                  /* long options toggles */
         break;

         case TRU_GETOPT_EXIT:
         return(TRU_GETOPT_EXIT);

         case TRU_GETOPT_ERROR:
         return(TRU_GETOPT_ERROR);

         case 'h':
         usage_func(cnf, short_opt);
         return(TRU_GETOPT_EXIT);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(TRU_GETOPT_ERROR);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", cnf->prog_name, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(TRU_GETOPT_ERROR);
      };
   };

   return(0);
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
   int   rc;
   int   c;
   int   opt;

   switch(c = getopt_long(argc, argv, short_opt, long_opt, opt_index))
   {
      case '4':
      cnf->tr_opts |= TRAD_IPV4;
      return(TRU_GETOPT_MATCHED);

      case '6':
      cnf->tr_opts |= TRAD_IPV6;
      return(TRU_GETOPT_MATCHED);

      case 'b':
      cnf->tr_opts |= TRAD_BUILTIN_DICT;
      return(TRU_GETOPT_MATCHED);

      case 'D':
      if ((rc = tinyrad_strsadd(&cnf->dict_files, optarg)) != TRAD_SUCCESS)
      {
         tru_error(cnf, rc, "out of virtual memory");
         return(TRU_GETOPT_ERROR);
      };
      return(TRU_GETOPT_MATCHED);

      case 'd':
      opt = ((optarg)) ? (int)strtol(optarg, NULL, 0) : TRAD_DEBUG_ANY;
      tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt);
      return(TRU_GETOPT_MATCHED);

      case 'f':
      cnf->attr_file = optarg;
      return(TRU_GETOPT_MATCHED);

      case 'H':
      cnf->url = optarg;
      return(TRU_GETOPT_MATCHED);

      case 'I':
      if ((rc = tinyrad_strsadd(&cnf->dict_paths, optarg)) != TRAD_SUCCESS)
      {
         tru_error(cnf, rc, "out of virtual memory");
         return(TRU_GETOPT_ERROR);
      };
      return(TRU_GETOPT_MATCHED);

      case 'n':
      cnf->opts |= TRU_OPT_DRY_RUN;
      return(TRU_GETOPT_MATCHED);

      case 'q':
      cnf->opts |=  TRUTILS_OPT_QUIET;
      cnf->opts &= ~TRUTILS_OPT_VERBOSE;
      return(TRU_GETOPT_MATCHED);

      case 'r':
      cnf->opts |= TRU_OPT_RESOLVE;
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
tru_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt )
{
   int                        x;
   const TinyRadUtilWidget *  widget;

   tru_usage_summary(cnf);
   tru_usage_options(short_opt);
   if (!(cnf->widget))
   {
      printf("WIDGETS:\n");
      for(x = 0; tru_widget_map[x].name != NULL; x++)
      {
         widget = &tru_widget_map[x];
         if ((widget->desc))
            printf("  %-25s %s\n", widget->name, widget->desc);
      };
   };
   printf("\n");
   return(0);
}


void
tru_usage_options(
         const char *                  short_opt )
{
   const char *   s;

   s = short_opt;

   printf("OPTIONS:\n");
   if ((strchr(s, '4'))) printf("  -4, --ipv4                use IPv4 addresses\n");
   if ((strchr(s, '6'))) printf("  -6, --ipv6                use IPv6 addresses\n");
   if ((strchr(s, 'b'))) printf("  -b, --builtin-dict        load built-in dictionary\n");
   if ((strchr(s, 'D'))) printf("  -D dictionary             include dictionary\n");
   if ((strchr(s, 'd'))) printf("  -d level, --debug=level   print debug messages\n");
   if ((strchr(s, 'f'))) printf("  -f file, --file=file      attribute list\n");
   if ((strchr(s, 'H'))) printf("  -H uri                    RADIUS URI\n");
   if ((strchr(s, 'h'))) printf("  -h, --help                print this help and exit\n");
   if ((strchr(s, 'I'))) printf("  -I path                   add path to dictionary search paths\n");
   if ((strchr(s, 'n'))) printf("  -n, --dry-run             generate request without transmitting request\n");
   if ((strchr(s, 'q'))) printf("  -q, --quiet, --silent     do not print messages\n");
   if ((strchr(s, 'r'))) printf("  -r, --resolve             resolve URLs\n");
   if ((strchr(s, 'V'))) printf("  -V, --version             print version number and exit\n");
   if ((strchr(s, 'v'))) printf("  -v, --verbose             print verbose messages\n");

   return;
}


void
tru_usage_summary(
         TinyRadUtilConf *                 cnf )
{
   const char *   widget_name;
   const char *   widget_help;

   widget_name  = (!(cnf->widget)) ? "widget" : cnf->widget->name;
   widget_help  = "";
   if ((cnf->widget))
      widget_help = ((cnf->widget->usage)) ? cnf->widget->usage : "";

   printf("Usage: %s [OPTIONS] %s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);
   printf("       %s-%s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);
   printf("       %s%s [OPTIONS]%s\n", PROGRAM_NAME, widget_name, widget_help);

   return;
}


static const TinyRadUtilWidget *
tru_widget_lookup(
         const char *                     wname,
         int                              exact )
{
   size_t                     wname_len;
   size_t                     x;
   size_t                     y;
   size_t                     len;
   const char *               alias;
   const TinyRadUtilWidget *  widget;
   const TinyRadUtilWidget *  match;

   len = strlen(PROGRAM_NAME);
   if (!(strncasecmp(wname, PROGRAM_NAME, len)))
      wname = &wname[len];
   if (wname[0] == '-')
      wname = &wname[1];
   if (!(wname[0]))
      return(NULL);

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
         if ( ((match)) && (match != widget) )
            return(NULL);
         if (exact == TRAD_NO)
            match = widget;
      };

      if (!(widget->aliases))
         continue;

      for(y = 0; ((widget->aliases[y])); y++)
      {
         alias = widget->aliases[y];
         if (!(strncmp(alias, wname, wname_len)))
         {
            if (alias[wname_len] == '\0')
               return(widget);
            if ( ((match)) && (match != widget) )
               return(NULL);
            if (exact == TRAD_NO)
               match = widget;
         };
      };
   };

   return(match);
}

/* end of source */
