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
#define _TESTS_TINYRAD_DICT_VALUES_C 1


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
#include <assert.h>

#include <tinyrad.h>

#include "dict-data.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-dict-values"


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
my_verify_value(
         unsigned                      opts,
         const TinyRadDictValueDef *   value_def,
         TinyRadDictAttr *             attr,
         TinyRadDictValue *            value );


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
   TinyRadDict *                 dict;
   TinyRadDictAttr *             attr;
   TinyRadDictValue *            value;
   const TinyRadDictValueDef *   value_def;

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

   opts  = TRAD_TEST_DFLT_DICT;

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
         opts |=  TRAD_TEST_QUIET;
         opts &= ~TRAD_TEST_VERBOSE;
         break;

         case 'V':
         trutils_version(PROGRAM_NAME);
         return(0);

         case 'v':
         opts |=  TRAD_TEST_VERBOSE;
         opts &= ~TRAD_TEST_QUIET;
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
   if ((our_dict_initialize(opts, &dict)))
      return(1);

   our_verbose(opts, "                                 %4s %9s %7s  %s", "Type", "Vendor", "Vendor", "Name");
   our_verbose(opts, "                                 %4s %9s %7s  %s",     "",     "ID",   "Type",     "");

   // verifies test values in dictionary by name
   for(pos = 0; ((test_dict_data_values[pos].value_name)); pos++)
   {
      value_def = &test_dict_data_values[pos];
      our_verbose(
         opts,
         "test    attribute search by name %25s %25s %3" PRIu64 "  ...",
         value_def->attr_name,
         value_def->value_name,
         value_def->data
      );
      if ((attr = tinyrad_dict_attr_get(dict, value_def->attr_name, 0, NULL, 0, 0)) == NULL)
      {
         our_error(opts, NULL, "%s: %s: unable to retrieve attribute", value_def->attr_name, value_def->value_name);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      value = tinyrad_dict_value_get(dict, value_def->value_name, attr, 0, 0, 0, 0);
      if ((my_verify_value(opts, value_def, attr, value)))
      {
         tinyrad_free(value);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      tinyrad_free(value);
      tinyrad_free(attr);
   };

   // verifies test values in dictionary by data
   for(pos = 0; ((test_dict_data_values[pos].value_name)); pos++)
   {
      value_def = &test_dict_data_values[pos];
      our_verbose(
         opts,
         "test    attribute search by name %25s %25s %3" PRIu64 "  ...",
         value_def->attr_name,
         value_def->value_name,
         value_def->data
      );
      if ((attr = tinyrad_dict_attr_get(dict, value_def->attr_name, 0, NULL, 0, 0)) == NULL)
      {
         our_error(opts, NULL, "%s: %s: unable to retrieve attribute", value_def->attr_name, value_def->value_name);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      value = tinyrad_dict_value_get(dict, NULL, attr, 0, 0, 0, value_def->data);
      if ((my_verify_value(opts, value_def, attr, value)))
      {
         tinyrad_free(value);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      tinyrad_free(value);
      tinyrad_free(attr);
   };

   // verifies default attributes in dictionary by name
   for(pos = 0; ((tinyrad_dict_default_values[pos].attr_name)); pos++)
   {
      value_def = &tinyrad_dict_default_values[pos];
      our_verbose(
         opts,
         "test    attribute search by name %25s %25s %3" PRIu64 "  ...",
         value_def->attr_name,
         value_def->value_name,
         value_def->data
      );
      if ((attr = tinyrad_dict_attr_get(dict, value_def->attr_name, 0, NULL, 0, 0)) == NULL)
      {
         our_error(opts, NULL, "%s: %s: unable to retrieve attribute", value_def->attr_name, value_def->value_name);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      value = tinyrad_dict_value_get(dict, value_def->value_name, attr, 0, 0, 0, 0);
      if ((my_verify_value(opts, value_def, attr, value)))
      {
         tinyrad_free(value);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      tinyrad_free(value);
      tinyrad_free(attr);
   };

   // verifies default attributes in dictionary by type
   for(pos = 0; ((tinyrad_dict_default_values[pos].attr_name)); pos++)
   {
      value_def = &tinyrad_dict_default_values[pos];
      our_verbose(
         opts,
         "test    attribute search by name %25s %25s %3" PRIu64 "  ...",
         value_def->attr_name,
         value_def->value_name,
         value_def->data
      );
      if ((attr = tinyrad_dict_attr_get(dict, value_def->attr_name, 0, NULL, 0, 0)) == NULL)
      {
         our_error(opts, NULL, "%s: %s: unable to retrieve attribute", value_def->attr_name, value_def->value_name);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      value = tinyrad_dict_value_get(dict, NULL, attr, 0, 0, 0, value_def->data);
      if ((my_verify_value(opts, value_def, attr, value)))
      {
         tinyrad_free(value);
         tinyrad_free(attr);
         tinyrad_free(dict);
         return(1);
      };
      tinyrad_free(value);
      tinyrad_free(attr);
   };

   // cleans up dictionary
   tinyrad_free(dict);

   return(0);
}


int
my_verify_value(
         unsigned                      opts,
         const TinyRadDictValueDef *   value_def,
         TinyRadDictAttr *             attr,
         TinyRadDictValue *            value )
{
   //char *      str;
   uint8_t     a_u8;
   uint8_t     v_u8;
   uint32_t    a_u32;
   uint32_t    v_u32;
   uint64_t    u64;

   assert(value_def  != NULL);
   assert(attr       != NULL);

   if (!(value))
   {
      our_dict_diag_value(opts, value_def, value);
      return(our_error(opts, NULL, "value was not found"));
   };

   //// compare value names
   //tinyrad_dict_value_info(value, TRAD_DICT_OPT_NAME, &str);
   //if ((strcasecmp(def->value_name, str)))
   //{
   //   tinyrad_free(str);
   //   our_dict_diag_value(opts, def, value);
   //   return(our_error(opts, NULL, "value names do not match"));
   //};
   //tinyrad_free(str);

   //// compare attribute names
   //tinyrad_dict_value_info(value, TRAD_DICT_OPT_ATTR_NAME, &str);
   //if ((strcasecmp(def->attr_name, str)))
   //{
   //   tinyrad_free(str);
   //   our_dict_diag_value(opts, def, value);
   //   return(our_error(opts, NULL, "value names do not match"));
   //};
   //tinyrad_free(str);

   // compare attribute type
   tinyrad_dict_attr_info(  attr,  TRAD_DICT_OPT_TYPE, &a_u8);
   tinyrad_dict_value_info( value, TRAD_DICT_OPT_TYPE, &v_u8);
   if (a_u8 != v_u8)
   {
      our_dict_diag_value(opts, value_def, value);
      return(our_error(opts, NULL, "attribute type does not match"));
   };

   // compare attribute vendor ID
   tinyrad_dict_attr_info(  attr,  TRAD_DICT_OPT_VEND_ID, &a_u32);
   tinyrad_dict_value_info( value, TRAD_DICT_OPT_VEND_ID, &v_u32);
   if (a_u32 != v_u32)
   {
      our_dict_diag_value(opts, value_def, value);
      return(our_error(opts, NULL, "attribute vendor id does not match"));
   };

   // compare attribute vendor type
   tinyrad_dict_attr_info(  attr,  TRAD_DICT_OPT_VEND_TYPE, &a_u32);
   tinyrad_dict_value_info( value, TRAD_DICT_OPT_VEND_TYPE, &v_u32);
   if (a_u32 != v_u32)
   {
      our_dict_diag_value(opts, value_def, value);
      return(our_error(opts, NULL, "attribute vendor type does not match"));
   };

   // compare value data
   tinyrad_dict_value_info(value, TRAD_DICT_OPT_DATA, &u64);
   if (value_def->data != u64)
   {
      our_dict_diag_value(opts, value_def, value);
      return(our_error(opts, NULL, "value data does not match"));
   };

   return(0);
}


/* end of source */

