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
#define _TESTS_TINYRAD_DICT_VENDORS_C 1


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
#define PROGRAM_NAME "tinyrad-dict-vendors"


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
my_verify_vendor(
         unsigned                      opts,
         const TinyRadDictVendorDef *  vendor_def,
         TinyRadDictVendor *           vendor );


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
   TinyRadDictVendor *           vendor;
   const TinyRadDictVendorDef *  vendor_def;

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

   our_verbose(opts, "Vendor format: vendor id/name");

   // verifies default attributes in dictionary by name
   for(pos = 0; ((tinyrad_dict_default_vendors[pos].name)); pos++)
   {
      vendor_def = &tinyrad_dict_default_vendors[pos];
      our_verbose( opts, "default attribute search by name %8" PRIu32 "  %s ...", vendor_def->vendor_id, vendor_def->name);
      vendor = tinyrad_dict_vendor_get(dict, vendor_def->name, 0);
      if ((my_verify_vendor(opts, vendor_def, vendor)))
      {
         tinyrad_free(vendor);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies test attributes in dictionary by type
   for(pos = 0; ((tinyrad_dict_default_vendors[pos].name)); pos++)
   {
      vendor_def = &tinyrad_dict_default_vendors[pos];
      our_verbose( opts, "default attribute search by id   %8" PRIu32 "  %s ...", vendor_def->vendor_id, vendor_def->name);
      vendor = tinyrad_dict_vendor_get(dict, NULL, (uint32_t)vendor_def->vendor_id);
      if ((my_verify_vendor(opts, vendor_def, vendor)))
      {
         tinyrad_free(vendor);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies test attributes in dictionary by name
   for(pos = 0; ((test_dict_data_vendors[pos].name)); pos++)
   {
      vendor_def = &test_dict_data_vendors[pos];
      our_verbose( opts, "test attribute search by name %8" PRIu32 "  %s ...", vendor_def->vendor_id, vendor_def->name);
      vendor = tinyrad_dict_vendor_get(dict, vendor_def->name, 0);
      if ((my_verify_vendor(opts, vendor_def, vendor)))
      {
         tinyrad_free(vendor);
         tinyrad_free(dict);
         return(1);
      };
   };

   // verifies default attributes in dictionary by type
   for(pos = 0; ((test_dict_data_vendors[pos].name)); pos++)
   {
      vendor_def = &test_dict_data_vendors[pos];
      our_verbose( opts, "test attribute search by id   %8" PRIu32 "  %s ...", vendor_def->vendor_id, vendor_def->name);
      vendor = tinyrad_dict_vendor_get(dict, NULL, (uint32_t)vendor_def->vendor_id);
      if ((my_verify_vendor(opts, vendor_def, vendor)))
      {
         tinyrad_free(vendor);
         tinyrad_free(dict);
         return(1);
      };
   };

   // cleans up dictionary
   tinyrad_free(dict);

   return(0);
}


int
my_verify_vendor(
         unsigned                      opts,
         const TinyRadDictVendorDef *  vendor_def,
         TinyRadDictVendor *           vendor )
{
   uint8_t     u8;
   uint32_t    u32;

   assert(vendor_def != NULL);

   if (!(vendor))
   {
      our_dict_diag_vendor(opts, vendor_def, NULL);
      return(our_error(opts, NULL, "vendor was not found"));
   };

   // compare vendor ID
   tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_VEND_ID, &u32);
   if (vendor_def->vendor_id != u32)
   {
      our_dict_diag_vendor(opts, vendor_def, vendor);
      return(our_error(opts, NULL, "vendor ID do not match"));
   };

   // compare vendor type octets
   tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_TYPE_OCTS, &u8);
   if (vendor_def->vendor_type_octs != u8)
   {
      our_dict_diag_vendor(opts, vendor_def, vendor);
      return(our_error(opts, NULL, "attribute type octets do not match"));
   };

   // compare vendor len octets
   tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_LEN_OCTS, &u8);
   if (vendor_def->vendor_len_octs != u8)
   {
      our_dict_diag_vendor(opts, vendor_def, vendor);
      return(our_error(opts, NULL, "attribute len octets do not match"));
   };

   return(0);
}


/* end of source */

