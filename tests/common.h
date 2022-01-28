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
#ifndef _TESTS_COMMON_H
#define _TESTS_COMMON_H 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <tinyrad_compat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tinyrad.h>
#include <tinyrad_common.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT "unknown"
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT "unknown"
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "Tiny RADIUS Client Library"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION "unknown"
#endif


#define TRAD_TEST_VERBOSE     0x0001UL
#define TRAD_TEST_RESOLVE     0x0002UL
#define TRAD_TEST_DESC2STR    0x0004UL
#define TRAD_TEST_PARSE       0x0008UL
#define TRAD_TEST_CHECK       0x0010UL
#define TRAD_TEST_QUIET       0x0020UL
#define TRAD_TEST_DFLT_DICT   0x0040UL


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

extern const char * prog_name;
extern const char * test_urldesc_strs_bad[];
extern const char * test_urldesc_strs_desc2str[];
extern const char * test_urldesc_strs_good[];
extern const char * test_urldesc_strs_multiple[];
extern const char * test_urldesc_strs_resolvable[];


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//-------------------//
// URLDesc functions //
//-------------------//
#pragma mark URLDesc functions

int our_urldesc_test_bad(const char * url, unsigned opts);
int our_urldesc_test_good(const char * url, unsigned opts);


//---------------//
// log functions //
//---------------//
#pragma mark log functions

int  our_error(unsigned opts, char ** errs, const char * fmt, ...);
void our_print(unsigned opts, const char * fmt, ...);
void our_verbose(unsigned opts, const char * fmt, ...);


//----------------//
// misc functions //
//----------------//
#pragma mark misc functions

void
our_initialize(
         const char *                  program_name );


char *
our_random_str(
         size_t                        min,
         size_t                        max );

#endif /* end of header */
