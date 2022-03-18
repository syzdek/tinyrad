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
#define _LIB_LIBTINYRAD_LCONF_C 1
#include "lconf.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <assert.h>

#include "ldict.h"
#include "lstrings.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#ifndef SYSCONFDIR
#   define SYSCONFDIR "/etc"
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

int
tinyrad_conf_environment(
         TinyRad *                     tr,
         TinyRadDict *                 dict );


int
tinyrad_conf_file(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         const char *                  file );


int
tinyrad_conf_init(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         unsigned                      opts );


int
tinyrad_conf_opt(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         const char *                  name,
         const char *                  value );


int
tinyrad_conf_opt_bool(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         unsigned                      opt,
         const char *                  value );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tinyrad_conf(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         unsigned                      opts )
{
   const char *      suffix;
   const char *      filename;
   const char *      prefix;
   char              buff[4096];
   char              path[128];
   struct passwd     pwd;
   struct passwd *   pwres;

   TinyRadDebugTrace();

   assert( ((tr)) || ((dict)) );

   // exit if init is disabled
   if ( ((opts & TRAD_NOINIT)) || ((getenv("TINYRADNOINIT"))) )
      return(TRAD_SUCCESS);

   // process environment variables
   tinyrad_conf_environment(tr, dict);

   // determine TINYRADRC suffix
   if ((suffix = getenv("TINYRADRC")) == NULL)
      suffix = "tinyradrc";

   // search current directory
   if ((getcwd(path, sizeof(path))))
   {
      tinyrad_strlcat(path, "/",    sizeof(path));
      tinyrad_strlcat(path, suffix, sizeof(path));
      tinyrad_conf_file(tr, dict, path);
   };

   // lookup user
   getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &pwres);
   prefix = (((pwres)) ? pwres->pw_dir : "/");

   // search for hidden RC file in home directory
   tinyrad_strlcpy(path, prefix, sizeof(path));
   tinyrad_strlcat(path, "/.",   sizeof(path));
   tinyrad_strlcat(path, suffix, sizeof(path));
   tinyrad_conf_file(tr, dict, path);

   // search for RC file in home directory
   tinyrad_strlcpy(path, prefix, sizeof(path));
   tinyrad_strlcat(path, "/",    sizeof(path));
   tinyrad_strlcat(path, suffix, sizeof(path));
   tinyrad_conf_file(tr, dict, path);

   // load global configuration
   if ((filename = getenv("TINYRADCONF")) == NULL)
      filename = SYSCONFDIR "/tinyrad.conf";
   tinyrad_conf_file(tr, dict, filename);

   return(TRAD_SUCCESS);
}


int
tinyrad_conf_environment(
         TinyRad *                     tr,
         TinyRadDict *                 dict )
{
   char *      value;

   if ((value = getenv("TINYRAD_BIND_ADDRESS")) != NULL)
      tinyrad_conf_opt(tr, dict, "bind_address", value);

   if ((value = getenv("TINYRAD_BUILTIN_DICTIONARY")) != NULL)
      tinyrad_conf_opt(tr, dict, "builtin_dictionary", value);

   if ((value = getenv("TINYRAD_DICTIONARY")) != NULL)
      tinyrad_conf_opt(tr, dict, "dictionary", value);

   if ((value = getenv("TINYRAD_NETWORK_TIMEOUT")) != NULL)
      tinyrad_conf_opt(tr, dict, "network_timeout", value);

   if ((value = getenv("TINYRAD_PATHS")) != NULL)
      tinyrad_conf_opt(tr, dict, "paths", value);

   if ((value = getenv("TINYRAD_SECRET")) != NULL)
      tinyrad_conf_opt(tr, dict, "secret", value);

   if ((value = getenv("TINYRAD_TIMEOUT")) != NULL)
      tinyrad_conf_opt(tr, dict, "timeout", value);

   if ((value = getenv("TINYRAD_URI")) != NULL)
      tinyrad_conf_opt(tr, dict, "uri", value);

   return(TRAD_SUCCESS);
}


int
tinyrad_conf_file(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         const char *                  file )
{
   int            fd;
   int            rc;
   int            argc;
   char           buff[TRAD_LINE_MAX_LEN];
   char           value[TRAD_LINE_MAX_LEN];
   const char *   val;
   size_t         len;
   char **        argv;

   TinyRadDebugTrace();

   assert( ((tr)) || ((dict)) );
   assert(file != NULL);

   len = 1;
   rc  = TRAD_SUCCESS;

   if ((fd = open(file, O_RDONLY)) == -1)
      return(TRAD_SUCCESS);

   while( ((len)) && (rc == TRAD_SUCCESS) )
   {
      if ((rc = tinyrad_readline(fd, buff, sizeof(buff), &len)) != TRAD_SUCCESS)
         continue;
      if (!(len))
         continue;
      if ((rc = tinyrad_strtoargs(buff, &argv, &argc)) != TRAD_SUCCESS)
      {
         if (rc == TRAD_ESYNTAX)
            rc = TRAD_SUCCESS;
         continue;
      };
      if ( (argc < 1) || (argc > 2) )
      {
         tinyrad_strsfree(argv);
         continue;
      };
      val = tinyrad_strexpand(value, argv[1], sizeof(value), TRAD_NO);
      rc = tinyrad_conf_opt(tr, dict, argv[0], val);
      tinyrad_strsfree(argv);
   };

   close(fd);

   if (rc != TRAD_SUCCESS)
      return(rc);

   return(TRAD_SUCCESS);
}


int
tinyrad_conf_opt(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         const char *                  name,
         const char *                  value )
{
   int               i;
   int               rc;
   char *            endptr;
   char **           strs;
   struct timeval    tv;

   assert( ((tr)) || ((dict)) );
   assert(name != NULL);
   assert(value != NULL);

   if (!(strcasecmp(name, "bind_address")))
   {
      if ( (!(tr)) || ((tr->bind_sa)) || ((tr->bind_sa6)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, value));
   };

   if (!(strcasecmp(name, "builtin_dictionary")))
   {
      tinyrad_conf_opt_bool(NULL, dict, TRAD_BUILTIN_DICT, value);
      return(TRAD_SUCCESS);
   };

   if (!(strcasecmp(name, "dictionary")))
   {
      if (!(dict))
         return(TRAD_SUCCESS);
      if ((dict->default_dictfile))
         return(TRAD_SUCCESS);
      if ((dict->default_dictfile = tinyrad_strdup(value)) == NULL)
         return(TRAD_ENOMEM);
      return(TRAD_SUCCESS);
   };

   if (!(strcasecmp(name, "network_timeout")))
   {
      if ( (!(tr)) || ((tr->net_timeout)) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((i = (int)strtoll(value, &endptr, 10)) < 1)
         return(TRAD_SUCCESS);
      if ((endptr[0]))
         return(TRAD_SUCCESS);
      memset(&tv, 0, sizeof(struct timeval));
      return(tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, &tv));
   };

   if (!(strcasecmp(name, "paths")))
   {
      if ( (!(dict)) || ((dict->paths)) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((rc = tinyrad_strsplit(value, ':', &strs, &i)) != TRAD_SUCCESS)
         return(rc);
      if (!(i))
      {
         tinyrad_strsfree(strs);
         return(TRAD_SUCCESS);
      };
      dict->paths = strs;
      return(TRAD_SUCCESS);
   };

  if (!(strcasecmp(name, "secret")))
   {
      if ( (!(tr)) || ((tr->secret)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_SECRET, value));
   };

   if (!(strcasecmp(name, "timeout")))
   {
      if ( (!(tr)) || (tr->timeout != -1) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((i = (int)strtoll(value, &endptr, 10)) < 1)
         return(TRAD_SUCCESS);
      if ((endptr[0]))
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, &i));
   };

   if (!(strcasecmp(name, "uri")))
   {
      if ( (!(tr)) || ((tr->trud)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_URI, value));
   };

   return(TRAD_SUCCESS);
}


int
tinyrad_conf_opt_bool(
         TinyRad *                     tr,
         TinyRadDict *                 dict,
         unsigned                      opt,
         const char *                  value )
{
   int      boolean;

   if ( (!(tr)) && (!(dict)) )
      return(TRAD_SUCCESS);

   if (!(value))
      boolean = TRAD_YES;
   else if (!(strcasecmp(value, "yes")))
      boolean = TRAD_YES;
   else if (!(strcasecmp(value, "no")))
      boolean = TRAD_NO;
   else
      return(TRAD_SUCCESS);

   if ((dict))
   {
      if ( ((dict->opts | dict->opts_neg) & opt) != 0)
      {
         if (boolean == TRAD_YES)
            dict->opts |= opt;
         if (boolean == TRAD_NO)
            dict->opts_neg |= opt;
      };
   };

   if ((tr))
   {
      if ( ((tr->opts | tr->opts_neg) & opt) != 0)
      {
         if (boolean == TRAD_YES)
            tr->opts |= opt;
         if (boolean == TRAD_NO)
            tr->opts_neg |= opt;
      };
   };

   return(TRAD_SUCCESS);
}

/* end of source */
