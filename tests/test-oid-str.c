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
#define _TESTS_TINYRAD_OID_STR_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "common.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <getopt.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "test-oid-str"


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

struct test_data
{
   size_t      len;
   uint32_t    vals[8];
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

struct test_data test_vals[] =
{
   { 1, { TRAD_ATTR_USER_NAME } },
   { 1, { TRAD_ATTR_USER_PASSWORD } },
   { 3, { TRAD_ATTR_VENDOR_SPECIFIC,      27893,     1 } },
   { 3, { TRAD_ATTR_VENDOR_SPECIFIC,      27893,   254 } },
   { 4, { TRAD_ATTR_EXTENDED_ATTRIBUTE_1,    26, 27893,   1 } },
   { 4, { TRAD_ATTR_EXTENDED_ATTRIBUTE_1,    26, 27893, 254 } },
   { 0, { 0 } }
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int main(
         int                           argc,
         char *                        argv[] );


char *
my_oidstr(
         uint32_t *                    vals,
         size_t                        len );


int
my_oid_test(
         int                           opts,
         TinyRadOID *                  oldoid,
         int                           type,
         const char *                  title );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int                  opt;
   int                  c;
   int                  opt_index;
   unsigned             opts;
   int                  errs;
   int                  rc;
   size_t               pos;
   size_t               x;
   size_t               y;
//   char *               str;
   TinyRadOID *         oid;
//   TinyRadOID *         oid2;
   uint32_t *           vals;
   size_t               len;

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

   opts = 0;
   errs = 0;

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

   for(x = 0; ((test_vals[x].len)); x++)
   {
      for(y = 1; (y <= test_vals[x].len); y++)
      {
         our_verbose(opts, "testing oid %s ...", my_oidstr(test_vals[x].vals, y));
         our_verbose(opts, "   Initializing OID from values ...");
         if ((oid = tinyrad_oid_init(test_vals[x].vals, y)) == NULL)
         {
            our_error(opts, NULL, "out of virtual memory");
            errs++;
            continue;
         };

         our_verbose(opts, "   Testing values of OID ...");
         if ((rc = tinyrad_oid_values(oid, &vals, &len)) != TRAD_SUCCESS)
         {
            our_error(opts, NULL, "tinyrad_oid_values(): %s", tinyrad_strerror(rc));
            errs++;
            tinyrad_free(oid);
            continue;
         };
         if (len != y)
         {
            our_error(opts, NULL, "OID length does not match reference");
            errs++;
            tinyrad_free(oid);
            continue;
         };
         for(pos = 0; (pos < y); pos++)
         {
            if (test_vals[x].vals[pos] != vals[pos])
            {
               our_error(opts, NULL, "OID values do not match reference");
               errs++;
               pos = y;
            };
         };
         tinyrad_free(vals);

         if ((my_oid_test(opts, oid, TRAD_OID_TYPE_ATTRIBUTE, "canonical attribute name")))
            errs++;

         if ((my_oid_test(opts, oid, TRAD_OID_TYPE_VENDOR, "canonical vendor name")))
            errs++;

         if ((my_oid_test(opts, oid, TRAD_OID_TYPE_VALUE, "canonical value name")))
            errs++;

         if ((my_oid_test(opts, oid, TRAD_OID_TYPE_NONE, "OID string")))
            errs++;

         if ((my_oid_test(opts, oid, 0xffff, "unknown canonical name")))
            errs++;

         tinyrad_free(oid);
      };
   };

   return( (((errs)) ? 1 : 0) );
}


char *
my_oidstr(
         uint32_t *                    vals,
         size_t                        len )
{
   size_t      pos;
   size_t      str_len;
   static char str[128];

   str[0]   = '\0';
   str_len  = 0;

   for(pos = 0; (pos < len); pos++)
   {
      if (str_len >= sizeof(str))
         continue;
      str_len += snprintf(&str[str_len], (sizeof(str)-str_len-1), ".%"PRIu32, vals[pos]);
   };
   str[sizeof(str)-1] = '\0';

   return(&str[1]);
}


int
my_oid_test(
         int                           opts,
         TinyRadOID *                  oldoid,
         int                           type,
         const char *                  title )
{
   TinyRadOID *      newoid;
   char *            str;
   size_t            pos;
   size_t            newlen;
   size_t            oldlen;
   uint32_t *        newvals;
   uint32_t *        oldvals;

   our_verbose(opts, "   Generating %s ...", title);
   if ((str = tinyrad_oid2str(oldoid, type)) == NULL)
   {
      our_error(opts, NULL, "out of virtual memory");
      return(1);
   };
   our_verbose(opts, "      %s:  %s", title, str);

   our_verbose(opts, "      Generating OID from %s ...", title);
   if ((newoid = tinyrad_str2oid(str)) == NULL)
   {
      our_error(opts, NULL, "error generating OID from %s", title);
      tinyrad_free(str);
      return(1);
   };

   tinyrad_free(str);

   tinyrad_oid_values(newoid, &newvals, &newlen);
   tinyrad_oid_values(oldoid, &oldvals, &oldlen);
   our_verbose(opts, "      Generated OID: %s", my_oidstr(newvals, newlen));
   if (newlen != oldlen)
   {
      our_error(opts, NULL, "old and new OID lengths do not match");
      tinyrad_free(newoid);
      tinyrad_free(newvals);
      tinyrad_free(oldvals);
      return(1);
   };
   for(pos = 0; (pos < newlen); pos++)
   {
      if (newvals[pos] != oldvals[pos])
      {
         our_error(opts, NULL, "old and new OID values do not match");
         tinyrad_free(newoid);
         tinyrad_free(newvals);
         tinyrad_free(oldvals);
         return(1);
      };
   };

   our_verbose(opts, "      comparing old OID and new OID ...");
   if ((tinyrad_oid_cmp(&oldoid, &newoid)))
   {
      tinyrad_free(newoid);
      our_error(opts, NULL, "old OID does not match new OID");
      return(1);
   };

   tinyrad_free(newoid);

   return(0);
}


/* end of source */
