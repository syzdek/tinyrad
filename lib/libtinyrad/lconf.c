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
#include "lmap.h"
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

#define TRAD_CONF_BIND_ADDRESS               1
#define TRAD_CONF_BUILTIN_DICTIONARY         2
#define TRAD_CONF_DICTIONARY                 3
#define TRAD_CONF_NETWORK_TIMEOUT            4
#define TRAD_CONF_PATHS                      5
#define TRAD_CONF_SECRET                     6
#define TRAD_CONF_SECRET_FILE                7
#define TRAD_CONF_STOPINIT                   8
#define TRAD_CONF_TIMEOUT                    9
#define TRAD_CONF_URI                        10


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tinyrad_conf_options[]
static const TinyRadMap tinyrad_conf_options[] =
{
   { "BIND_ADDRESS",          TRAD_CONF_BIND_ADDRESS },
   { "BUILTIN_DICTIONARY",    TRAD_CONF_BUILTIN_DICTIONARY },
   { "DICTIONARY",            TRAD_CONF_DICTIONARY },
   { "NETWORK_TIMEOUT",       TRAD_CONF_NETWORK_TIMEOUT },
   { "PATHS",                 TRAD_CONF_PATHS },
   { "SECRET",                TRAD_CONF_SECRET },
   { "SECRET_FILE",           TRAD_CONF_SECRET_FILE },
   { "STOPINIT",              TRAD_CONF_STOPINIT },
   { "TIMEOUT",               TRAD_CONF_TIMEOUT },
   { "URI",                   TRAD_CONF_URI },
   { NULL, 0 }
};


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
         uint64_t                      optid,
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
   const char *      tinradrc;
   const char *      tinyradconf;
   char              buff[4096];
   char              path[128];
   const char *      home;
   struct passwd     pwd;
   struct passwd *   pwres;

   TinyRadDebugTrace();

   assert( ((tr)) || ((dict)) );

   // exit if init is disabled
   if ( ((opts & TRAD_NOINIT)) || ((getenv("TINYRADNOINIT"))) )
      return(TRAD_SUCCESS);

   // process environment variables
   tinyrad_conf_environment(tr, dict);

   // lookup user
   getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &pwres);
   home = (((pwres)) ? pwres->pw_dir : "/");

   // determine TINYRADRC suffix
   if ((tinradrc = getenv("TINYRADRC")) != NULL)
   {
      // process "./${TINYRADRC}"
      if ((getcwd(path, sizeof(path))))
      {
         tinyrad_strlcat(path, "/",      sizeof(path));
         tinyrad_strlcat(path, tinradrc, sizeof(path));
         tinyrad_conf_file(tr, dict, path);
      };

      // process "~/.{$TINYRADRC}"
      tinyrad_strlcpy(path, home,     sizeof(path));
      tinyrad_strlcat(path, "/.",     sizeof(path));
      tinyrad_strlcat(path, tinradrc, sizeof(path));
      tinyrad_conf_file(tr, dict, path);

      // process "~/${TINYRADRC}"
      tinyrad_strlcpy(path, home,     sizeof(path));
      tinyrad_strlcat(path, "/",      sizeof(path));
      tinyrad_strlcat(path, tinradrc, sizeof(path));
      tinyrad_conf_file(tr, dict, path);
   };

   // process "${TINYRADCONF}"
   if ((tinyradconf = getenv("TINYRADCONF")) != NULL)
      tinyrad_conf_file(tr, dict, tinyradconf);

   // process "./tinyradrc"
   if ((getcwd(path, sizeof(path))))
   {
      tinyrad_strlcat(path, "/tinyradrc", sizeof(path));
      tinyrad_conf_file(tr, dict, path);
   };

   // process "~/.tinyradrc"
   tinyrad_strlcpy(path, home,            sizeof(path));
   tinyrad_strlcat(path, "/.tinyradrc",   sizeof(path));
   tinyrad_conf_file(tr, dict, path);

   // process "~/tinyradrc"
   tinyrad_strlcpy(path, home,         sizeof(path));
   tinyrad_strlcat(path, "/tinyradrc", sizeof(path));
   tinyrad_conf_file(tr, dict, path);

   // process "/usr/local/etc/tinyrad.conf"
   tinyrad_conf_file(tr, dict, SYSCONFDIR "/tinyrad.conf");

   return(TRAD_SUCCESS);
}


int
tinyrad_conf_environment(
         TinyRad *                     tr,
         TinyRadDict *                 dict )
{
   size_t                  pos;
   char *                  value;
   char                    varname[64];
   const TinyRadMap *      opt;

   TinyRadDebugTrace();

   for(pos = 0; ((tinyrad_conf_options[pos].map_name)); pos++)
   {
      opt = &tinyrad_conf_options[pos];
      if (opt->map_value == TRAD_CONF_STOPINIT)
         continue;
      tinyrad_strlcpy(varname, "TINYRAD_", sizeof(varname));
      tinyrad_strlcat(varname, opt->map_name, sizeof(varname));
      if ((value = getenv(varname)) != NULL)
         tinyrad_conf_opt(tr, dict, opt->map_value, value);
   };

   if ((value = getenv("TINYRAD_STOPINIT")) != NULL)
      tinyrad_conf_opt(tr, dict, TRAD_CONF_STOPINIT, value);

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
   uint64_t       optid;

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
      if ((optid = tinyrad_map_lookup_name(tinyrad_conf_options, argv[0], NULL)) > 0)
         rc = tinyrad_conf_opt(tr, dict, optid, val);
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
         uint64_t                      optid,
         const char *                  value )
{
   int               i;
   int               rc;
   char *            endptr;
   char **           strs;
   struct timeval    tv;

   assert( ((tr)) || ((dict)) );
   assert(value != NULL);

   TinyRadDebugTrace();

   tr   = ( ((tr))   && (!(tr->opts   & TRAD_STOPINIT)) ) ? tr   : NULL;
   dict = ( ((dict)) && (!(dict->opts & TRAD_STOPINIT)) ) ? dict : NULL;

   switch(optid)
   {
      case TRAD_CONF_BIND_ADDRESS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_BIND_ADDRESS, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->bind_sa)) || ((tr->bind_sa6)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, value));

      case TRAD_CONF_BUILTIN_DICTIONARY:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_BUILTIN_DICTIONARY, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      tinyrad_conf_opt_bool(NULL, dict, TRAD_BUILTIN_DICT, value);
      return(TRAD_SUCCESS);

      case TRAD_CONF_DICTIONARY:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_DICTIONARY, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if (!(dict))
         return(TRAD_SUCCESS);
      if ((dict->default_dictfile))
         return(TRAD_SUCCESS);
      if ((dict->default_dictfile = tinyrad_strdup(value)) == NULL)
         return(TRAD_ENOMEM);
      return(TRAD_SUCCESS);

      case TRAD_CONF_NETWORK_TIMEOUT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_NETWORK_TIMEOUT, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->net_timeout)) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((i = (int)strtoll(value, &endptr, 10)) < 1)
         return(TRAD_SUCCESS);
      if ((endptr[0]))
         return(TRAD_SUCCESS);
      memset(&tv, 0, sizeof(struct timeval));
      return(tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, &tv));

      case TRAD_CONF_PATHS:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_PATHS, \"%s\" )", __func__, (((value)) ? value : "(null)"));
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

      case TRAD_CONF_SECRET:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_SECRET, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->secret)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_SECRET, value));

      case TRAD_CONF_SECRET_FILE:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_SECRET_FILE, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->secret)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_SECRET_FILE, value));

      case TRAD_CONF_STOPINIT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_STOPINIT, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      tinyrad_conf_opt_bool(tr, dict, TRAD_STOPINIT, value);
      return(TRAD_SUCCESS);

      case TRAD_CONF_TIMEOUT:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_TIMEOUT, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || (tr->timeout != -1) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((i = (int)strtoll(value, &endptr, 10)) < 1)
         return(TRAD_SUCCESS);
      if ((endptr[0]))
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_TIMEOUT, &i));

      case TRAD_CONF_URI:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_URI, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->trud)) || (!(value)) )
         return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_URI, value));

      default:
      break;
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

   TinyRadDebugTrace();

   if ( (!(tr)) && (!(dict)) )
      return(TRAD_SUCCESS);

   if (!(value))                           boolean = TRAD_YES;
   else if (!(strcasecmp(value, "1")))     boolean = TRAD_YES;
   else if (!(strcasecmp(value, "true")))  boolean = TRAD_YES;
   else if (!(strcasecmp(value, "yes")))   boolean = TRAD_YES;
   else if (!(strcasecmp(value, "0")))     boolean = TRAD_NO;
   else if (!(strcasecmp(value, "false"))) boolean = TRAD_NO;
   else if (!(strcasecmp(value, "no")))    boolean = TRAD_NO;
   else return(TRAD_SUCCESS);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, %u, \"%s\" )", __func__, opt, ((boolean == TRAD_YES) ? "TRAD_YES" : "TRAD_NO"));

   if ( ((dict)) && ( ((dict->opts | dict->opts_neg) & opt) != 0) )
   {
      switch(boolean)
      {
         case TRAD_YES: dict->opts     |= opt; break;
         case TRAD_NO:  dict->opts_neg |= opt; break;
         default:                              break;
      };
   };

   if ( ((tr)) && ( ((tr->opts | tr->opts_neg) & opt) != 0) )
   {
      switch(boolean)
      {
         case TRAD_YES: tr->opts     |= opt; break;
         case TRAD_NO:  tr->opts_neg |= opt; break;
         default:                            break;
      };
   };

   return(TRAD_SUCCESS);
}

/* end of source */
