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
#ifndef _SRC_TRU_TINYRADUTIL_H
#define _SRC_TRU_TINYRADUTIL_H 1

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

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad"


#undef TRU_PREFIX
#define TRU_PREFIX "tinyrad"


#define TRU_GETOPT_MATCHED    -2
#define TRU_GETOPT_ERROR      -3
#define TRU_GETOPT_EXIT       -4


#define TRU_OPT_RESOLVE       0x0001
#define TRU_OPT_DRY_RUN       0x0002


#define TRU_COMMON_SHORT "d:hqVv"
#define TRU_COMMON_LONG \
   { "debug",            optional_argument, NULL, 'd' }, \
   { "help",             no_argument,       NULL, 'h' }, \
   { "quiet",            no_argument,       NULL, 'q' }, \
   { "silent",           no_argument,       NULL, 'q' }, \
   { "version",          no_argument,       NULL, 'V' }, \
   { "verbose",          no_argument,       NULL, 'v' }, \
   { NULL, 0, NULL, 0 }


#define TRU_DICT_SHORT "bD:I:"
#define TRU_DICT_LONG \
   { "builtin-dict",     no_argument,       NULL, 'b' }, \


#define TRU_NET_SHORT "H:46"
#define TRU_NET_LONG \
   { "ipv4",             no_argument,       NULL, '4' }, \
   { "ipv6",             no_argument,       NULL, '6' }, \


#define TRU_REQ_SHORT "f:n"
#define TRU_REQ_LONG \
   { "dry-run",          no_argument,       NULL, 'n' }, \
   { "file",             optional_argument, NULL, 'f' }, \


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct tinyrad_util_config     TinyRadUtilConf;
typedef struct tinyrad_util_widget     TinyRadUtilWidget;


typedef struct my_request_av
{
   char * attr_name;
   char * attr_value;
} MyRequestAV;


struct tinyrad_util_config
{
   unsigned                   opts;
   unsigned                   tr_opts;
   int                        padint;
   int                        argc;
   char **                    argv;
   const char *               widget_name;
   size_t                     widget_name_len;
   const char *               prog_name;
   const TinyRadUtilWidget *  widget;
   TinyRad *                  tr;
   const char *               url;
   char **                    dict_files;
   char **                    dict_paths;
   const char *               attr_file;
   void *                     padptr;
};


struct tinyrad_util_widget
{
   const char *               name;
   const char *               desc;
   const char *               usage;
   const char * const *       aliases;
   int  (*func)(TinyRadUtilConf * cnf);
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_cmdmap[]
extern const TinyRadUtilWidget tru_widget_map[];


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

//--------------------//
// logging prototypes //
//--------------------//
#pragma mark logging prototypes

int
tru_error(
         TinyRadUtilConf *                cnf,
         int                              rc,
         const char *                     fmt,
         ... );


int
tru_errors(
         TinyRadUtilConf *                cnf,
         int                              rc,
         char **                          errs );


int
tru_printf(
         TinyRadUtilConf *                cnf,
         const char *                     fmt,
         ... );


int
tru_verbose(
         TinyRadUtilConf *                cnf,
         const char *                     fmt,
         ... );


//--------------------------//
// miscellaneous prototypes //
//--------------------------//
#pragma mark miscellaneous prototypes

int
tru_load_tinyrad(
         TinyRadUtilConf *             cnf );


//------------------//
// usage prototypes //
//------------------//
#pragma mark usage prototypes

int
tru_cli_parse(
         TinyRadUtilConf *             cnf,
         int                           argc,
         char * const *                argv,
         const char *                  short_opt,
         const struct option *         long_opt,
         int (*usage_func)(TinyRadUtilConf * cnf, const char * short_opt ) );


int
tru_getopt(
         TinyRadUtilConf *             cnf,
         int                           argc,
         char * const *                argv,
         const char *                  short_opt,
         const struct option *         long_opt,
         int *                         opt_index );


int
tru_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt );


void
tru_usage_options(
         const char *                  short_opt );


void
tru_usage_summary(
         TinyRadUtilConf *             cnf );


//-------------------//
// widget prototypes //
//-------------------//
#pragma mark widget prototypes

int
tru_widget_config(
         TinyRadUtilConf *             cnf );


int
tru_widget_dict(
         TinyRadUtilConf *             cnf );


int
tru_widget_url(
         TinyRadUtilConf *             cnf );


#endif /* end of header */
