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
#define _SRC_TRU_WIDGET_CONFIG_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "tinyradutil.h"

#include <stdio.h>
#include <assert.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tru_widget_config(
         TinyRadUtilConf *                 cnf )
{
   int               rc;
   TinyRadDict *     dict;

   // getopt options
   static char          short_opt[] = TRU_COMMON_SHORT TRU_DICT_SHORT;
   static struct option long_opt[]  = { TRU_DICT_LONG TRU_COMMON_LONG };

   // process widget cli options
   if ((rc = tru_cli_parse(cnf, cnf->argc, cnf->argv, short_opt, long_opt)) != 0)
   {
      return((rc == 0) ? 0 : 1);
   };
   if (optind < cnf->argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, cnf->argv[optind+1]);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
      return(1);
   };

   if ((rc = tru_load_tinyrad(cnf)) != TRAD_SUCCESS)
      return(rc);
   tinyrad_get_option(cnf->tr, TRAD_OPT_DICTIONARY, &dict);
   tinyrad_conf_print(cnf->tr, dict);
   tinyrad_free(dict);

   return(TRAD_SUCCESS);
}


/* end of source */
