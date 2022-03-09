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
#define _LIB_LIBTRUTILS_LVERBOSE_C 1
#include "lverbose.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <time.h>
#include <assert.h>



///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark program_name
const char * program_name = "tinyrad-prog";


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions


void
trutils_initialize(
         const char *                  prog_name )
{
   int                  opt;
   struct timespec      ts;

   assert(prog_name != NULL);

   // initialize program_name
   program_name = prog_name;

   // initialize library debug
   opt = TRAD_OFF;
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt);
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_IDENT, program_name);

   // seed random()
   clock_gettime(CLOCK_MONOTONIC, &ts);
   srandom((unsigned)(ts.tv_sec + ts.tv_sec));

   return;
}


void
trutils_version( void )
{
   printf("%s (%s) %s\n", program_name, PACKAGE_NAME, PACKAGE_VERSION);
   printf("%s (%s) %s\n", tinyrad_lib_name(), tinyrad_pkg_name(), tinyrad_pkg_version(NULL, NULL, NULL, NULL));
   return;
}

/* end of source */
