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
#ifndef __TINYRAD_UTILS_H
#define __TINYRAD_UTILS_H 1


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
#include <tinyrad_internal.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#pragma mark - Macros


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


#define TRUTILS_OPT_VERBOSE   0x00010000UL
#define TRUTILS_OPT_QUIET     0x00020000UL


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _trutils_file_buffer TRUtilsFile;


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

extern const char * program_name;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//-----------------//
// file prototypes //
//-----------------//
#pragma mark file prototypes

_TINYRAD_F void
trutils_close(
         TRUtilsFile *                 fb );


_TINYRAD_F TRUtilsFile *
trutils_open(
         unsigned                      opts,
         const char *                  filename );


_TINYRAD_F int
trutils_readline(
         unsigned                      opts,
         TRUtilsFile *                 fb );


//--------------------//
// logging prototypes //
//--------------------//
#pragma mark logging prototypes

_TINYRAD_F int
trutils_error(
         unsigned                      opts,
         char **                       errs,
         const char *                  fmt,
         ... );


_TINYRAD_F void
trutils_initialize(
         const char *                  prog_name );


_TINYRAD_F void
trutils_print(
         unsigned                      opts,
         const char *                  fmt,
         ... );


_TINYRAD_F void
trutils_verbose(
         unsigned                      opts,
         const char *                  fmt,
         ... );


_TINYRAD_F void
trutils_version( void );


//--------------------------//
// miscellaneous prototypes //
//--------------------------//
#pragma mark miscellaneous prototypes

_TINYRAD_F int
trutils_exit_code(
         int                           rc );


_TINYRAD_F char *
trutils_strrand(
         size_t                        min,
         size_t                        max );


#endif /* end of header */
