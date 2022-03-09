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
#define _SRC_UTILITIES_TINYRAD_C 1


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


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct my_file_buffer
{
   int         fd;         // file descriptor
   int         padint;
   char *      eol;        // end of current line
   char *      bol;        // beginning of current line
   size_t      len;        // length of current buffer
   char        buff[256];  // raw buffer
   char *      filename;
} MyFile;


typedef struct my_request_av
{
   char * attr_name;
   char * attr_value;
} MyRequestAV;


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


void
my_file_close(
         MyFile *                      fb );


MyFile *
my_file_open(
         const char *                  filename );


int
my_file_readline(
         MyFile *                      fb );


void
my_usage( void );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main(int argc, char * argv[])
{
   int            opt;
   int            c;
   int            opt_index;
   int            rc;
   TinyRadDict *  dict;
   TinyRad *      tr;
   const char *   url;
   char **        errs;
   unsigned       opts;
   uint8_t        dictdump;
   uint8_t        dictloaded;

   // getopt options
   static char          short_opt[] = "D:d:f:hI:qVv";
   static struct option long_opt[] =
   {
      {"debug",            optional_argument, NULL, 'd' },
      {"file",             optional_argument, NULL, 'f' },
      {"help",             no_argument,       NULL, 'h' },
      {"quiet",            no_argument,       NULL, 'q' },
      {"silent",           no_argument,       NULL, 'q' },
      {"version",          no_argument,       NULL, 'V' },
      {"verbose",          no_argument,       NULL, 'v' },
      {"defaults",         no_argument,       NULL,  2  },
      {"dictionary-dump",  no_argument,       NULL,  1  },
      { NULL, 0, NULL, 0 }
   };

   trutils_initialize(PROGRAM_NAME);

   opts       = 0;
   dictdump   = 0;
   dictloaded = 0;

   if (tinyrad_dict_initialize(&dict, 0) != TRAD_SUCCESS)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
         break;

         case 1:
         dictdump++;
         break;

         case 2:
         if (tinyrad_dict_defaults(dict, &errs, 0) != TRAD_SUCCESS)
         {
            our_error(PROGRAM_NAME, errs, NULL);
            tinyrad_free(dict);
            return(1);
         };
         dictloaded++;
         break;

         case 'D':
         if (tinyrad_dict_parse(dict, optarg, &errs, 0) != TRAD_SUCCESS)
         {
            tinyrad_free(dict);
            our_error(PROGRAM_NAME, errs, NULL);
            tinyrad_strsfree(errs);
            return(1);
         };
         dictloaded++;
         break;

         case 'd':
         opt = ((optarg)) ? (int)strtol(optarg, NULL, 0) : TRAD_DEBUG_ANY;
         tinyrad_set_option(NULL, TRAD_OPT_DEBUG_LEVEL, &opt);
         break;

         case 'h':
         my_usage();
         return(0);

         case 'I':
         if ((rc = tinyrad_dict_add_path(dict, optarg)) != TRAD_SUCCESS)
         {
            tinyrad_free(dict);
            our_error(PROGRAM_NAME, NULL, "%s: %s", optarg, tinyrad_strerror(rc));
            return(1);
         };
         break;

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

   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   url = argv[optind];
   optind++;

   if (!(dictloaded))
   {
      if (tinyrad_dict_defaults(dict, &errs, 0) != TRAD_SUCCESS)
      {
         our_error(PROGRAM_NAME, errs, NULL);
         tinyrad_free(dict);
         return(1);
      };
   };

   if ((dictdump))
   {
      tinyrad_dict_print(dict, 0xffff);
      tinyrad_free(dict);
      return(0);
   };

   if ((tinyrad_initialize(&tr, dict, url, 0)) != TRAD_SUCCESS)
   {
      tinyrad_free(dict);
      return(1);
   };

   tinyrad_free(tr);
   tinyrad_free(dict);

   return(0);
}


void
my_file_close(
         MyFile *                      fb )
{
   if (!(fb))
      return;

   if (fb->fd != -1)
      close(fb->fd);

   if ((fb->filename))
      free(fb->filename);

   free(fb);

   return;
}


MyFile *
my_file_open(
         const char *                  filename )
{
   MyFile *       fb;
   struct stat    sb;
   int            rc;

   assert(filename != NULL);

   // verify file exists
   if ((rc = stat(filename, &sb)) == -1)
   {
      our_error(PROGRAM_NAME,  NULL, "%s: %s", filename, strerror(errno));
      return(NULL);
   };
   if ((sb.st_mode & S_IFMT) != S_IFREG)
   {
      our_error(PROGRAM_NAME,  NULL, "%s: is not a file", filename);
      return(NULL);
   };

   // allocate initial
   if ((fb = malloc(sizeof(MyFile))) == NULL)
   {
      our_error(PROGRAM_NAME,  NULL, "out of virtual memory");
      return(NULL);
   };
   memset(fb, 0, sizeof(MyFile));
   fb->fd = -1;

   // copy filename
   if ((fb->filename = strdup(filename)) == NULL)
   {
      our_error(PROGRAM_NAME,  NULL, "out of virtual memory");
      my_file_close(fb);
      return(NULL);
   };

   // open file
   if ((fb->fd = open(filename, O_RDONLY)) == -1)
   {
      our_error(PROGRAM_NAME,  NULL, "%s: %s", filename, strerror(errno));
      my_file_close(fb);
      return(NULL);
   };

   return(fb);
}


int
my_file_readline(
         MyFile *                      fb )
{
   size_t      offset;
   size_t      pos;
   size_t      size;
   ssize_t     rc;

   assert(fb != 0);

   // adjusts beginning of line
   fb->bol = ((fb->eol)) ? &fb->eol[1] : fb->buff;

   // look for end line
   if ((fb->eol = strchr(fb->bol, '\n')) != NULL)
   {
      fb->eol[0] = '\0';
      return(1);
   };

   // shift buffer
   offset = (size_t)(fb->bol = fb->buff);
   for(pos = 0; ((pos+offset) < fb->len); pos++)
      fb->buff[pos] = fb->buff[pos+offset];
   fb->buff[pos]  = '\0';
   fb->len       -= offset;

   // attempt to fill buffer
   size = sizeof(fb->buff) - fb->len - 1;
   if ((rc = read(fb->fd, &fb->buff[fb->len], size)) == -1)
   {
      our_error(PROGRAM_NAME, NULL, "%s: %s", fb->filename, strerror(errno));
      return(-1);
   };
   fb->len           += (size_t)rc;
   fb->buff[fb->len]  = '\0';
   fb->bol            = fb->buff;
   if (!(fb->len))
      return(0);

   // look for end line
   if ((fb->eol = strchr(fb->bol, '\n')) != NULL)
   {
      fb->eol[0] = '\0';
      return(1);
   };

   fb->eol = &fb->bol[strlen(fb->bol)];

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] url\n", PROGRAM_NAME);
   printf("OPTIONS:\n");
   printf("  -D dictionary             include dictionary\n");
   printf("  -d level, --debug=level   print debug messages\n");
   printf("  -f file, --file=file      attribute list\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -I path                   add path to dictionary search paths\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  --defaults                load default dictionaries\n");
   printf("  --dictionary-dump         print imported dictionaries\n");
   printf("\n");
   return;
}


/* end of source */
