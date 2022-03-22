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
#define _TESTS_TINYRAD_DICT_ATTRS_C 1


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

#include "common-dict.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-dict-attrs"


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
my_verify_attr(
         unsigned                      opts,
         const TinyRadDictAttrDef *    attr_def,
         TinyRadDictAttr *             attr );


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
   unsigned                      tr_opts;
   TinyRadDict *                 dict;
   TinyRadDictAttr *             attr;
   const TinyRadDictAttrDef *    attr_def;

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

   trutils_initialize(PROGRAM_NAME);

   opts     = 0;
   tr_opts  = TRAD_NOINIT | TRAD_BUILTIN_DICT;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 'd':
         opt = TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL,  &opt);
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

   // initialize test dictionary
   if ((our_dict_initialize(opts, tr_opts, &dict)))
      return(1);

   trutils_verbose(opts, "                                 %4s %9s %7s  %s", "Type", "Vendor", "Vendor", "Name");
   trutils_verbose(opts, "                                 %4s %9s %7s  %s",     "",     "ID",   "Type",     "");

   // verifies test attributes in dictionary by name
   for(pos = 0; ((test_dict_data_attrs[pos].name)); pos++)
   {
      attr_def = &test_dict_data_attrs[pos];
      trutils_verbose(
         opts,
         "test    attribute search by name %4" PRIu8 " %9" PRIu32 " %7" PRIu32 "  %s ...",
         attr_def->type,
         attr_def->vendor_id,
         attr_def->vendor_type,
         attr_def->name
      );
      attr = tinyrad_dict_attr_get(dict, attr_def->name, 0, NULL, 0, 0);
      if ((my_verify_attr(opts, attr_def, attr)))
      {
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies test attributes in dictionary by type
   for(pos = 0; ((test_dict_data_attrs[pos].name)); pos++)
   {
      attr_def = &test_dict_data_attrs[pos];
      trutils_verbose(
         opts,
         "test    attribute search by type %4" PRIu8 " %9" PRIu32 " %7" PRIu32 "  %s ...",
         attr_def->type,
         attr_def->vendor_id,
         attr_def->vendor_type,
         attr_def->name
      );
      attr = tinyrad_dict_attr_get(dict, NULL, (uint8_t)attr_def->type, NULL, attr_def->vendor_id, attr_def->vendor_type);
      if ((my_verify_attr(opts, attr_def, attr)))
      {
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies default attributes in dictionary by name
   for(pos = 0; ((tinyrad_dict_default_attrs[pos].name)); pos++)
   {
      attr_def = &tinyrad_dict_default_attrs[pos];
      trutils_verbose(
         opts,
         "default attribute search by name %4" PRIu8 " %9" PRIu32 " %7" PRIu32 "  %s ...",
         attr_def->type,
         attr_def->vendor_id,
         attr_def->vendor_type,
         attr_def->name
      );
      attr = tinyrad_dict_attr_get(dict, attr_def->name, 0, NULL, 0, 0);
      if ((my_verify_attr(opts, attr_def, attr)))
      {
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies default attributes in dictionary by type
   for(pos = 0; ((tinyrad_dict_default_attrs[pos].name)); pos++)
   {
      attr_def = &tinyrad_dict_default_attrs[pos];
      trutils_verbose(
         opts,
         "default attribute search by type %4" PRIu8 " %9" PRIu32 " %7" PRIu32 "  %s ...",
         attr_def->type,
         attr_def->vendor_id,
         attr_def->vendor_type,
         attr_def->name
      );
      attr = tinyrad_dict_attr_get(dict, NULL, (uint8_t)attr_def->type, NULL, attr_def->vendor_id, attr_def->vendor_type);
      if ((my_verify_attr(opts, attr_def, attr)))
      {
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
   };

   // cleans up dictionary
   tinyrad_free(dict);

   return(0);
}


int
my_verify_attr(
         unsigned                      opts,
         const TinyRadDictAttrDef *    attr_def,
         TinyRadDictAttr *             attr )
{
   uint8_t     u8;
   uint32_t    u32;

   assert(attr_def != NULL);

   if (!(attr))
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute was not found"));
   };

   // compare data types
   tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_DATA_TYPE, &u32);
   if (attr_def->data_type != u32)
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute data types do not match"));
   };

   // compare types
   tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_TYPE, &u8);
   if (attr_def->type != u8)
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute types do not match"));
   };

   // compare vendor ID
   tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_VEND_ID, &u32);
   if (attr_def->vendor_id != u32)
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute vendor ID do not match"));
   };

   // compare vendor types
   tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_VEND_TYPE, &u32);
   if (attr_def->vendor_type != u32)
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute vendor types do not match"));
   };

   // compare flags
   tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_FLAGS, &u32);
   if (attr_def->flags != u32)
   {
      our_dict_diag_attr(opts, attr_def, attr);
      return(trutils_error(opts, NULL, "attribute flags do not match"));
   };

   return(0);
}


/* end of source */

