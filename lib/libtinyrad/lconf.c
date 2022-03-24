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
#define TRAD_CONF_RANDOM                     11
#define TRAD_CONF_IPV4                       12
#define TRAD_CONF_IPV6                       13


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
   { "IPV4",                  TRAD_CONF_IPV4 },
   { "IPV6",                  TRAD_CONF_IPV6 },
   { "NETWORK_TIMEOUT",       TRAD_CONF_NETWORK_TIMEOUT },
   { "PATHS",                 TRAD_CONF_PATHS },
   { "RANDOM",                TRAD_CONF_RANDOM },
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
         unsigned                      flag,
         const char *                  value );


void
tinyrad_conf_print_bool(
         int                           comment,
         const char *                  name,
         int                           value );


void
tinyrad_conf_print_hex(
         int                           comment,
         const char *                  name,
         unsigned                      value );


void
tinyrad_conf_print_int(
         int                           comment,
         const char *                  name,
         int                           value);


void
tinyrad_conf_print_line(
         int                           comment,
         const char *                  name,
         const char *                  value );


void
tinyrad_conf_print_str(
         int                           comment,
         const char *                  name,
         const char *                  str );


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
      if ( (!(dict)) || (!(value)) )
         return(TRAD_SUCCESS);
      tinyrad_conf_opt_bool(NULL, dict, TRAD_BUILTIN_DICT, value);
      return(TRAD_SUCCESS);

      case TRAD_CONF_DICTIONARY:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_DICTIONARY, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(dict)) || ((dict->default_dictfile)) || (!(value)) )
         return(TRAD_SUCCESS);
      if ((dict->default_dictfile = tinyrad_strdup(value)) == NULL)
         return(TRAD_ENOMEM);
      return(TRAD_SUCCESS);

      case TRAD_CONF_IPV4:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_IPV4, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->opts & TRAD_IPV4)) || ((tr->opts_neg & TRAD_IPV4)) )
         return(TRAD_SUCCESS);
      switch(i = ((value)) ? tinyrad_strtobool(value) : TRAD_YES)
      {  case TRAD_NO:  tinyrad_set_option(tr, TRAD_OPT_IPV4, &i); break;
         case TRAD_YES: tinyrad_set_option(tr, TRAD_OPT_IPV4, &i); break;
         default: break;
      };
      return(TRAD_SUCCESS);

      case TRAD_CONF_IPV6:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_IPV6, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || ((tr->opts & TRAD_IPV6)) || ((tr->opts_neg & TRAD_IPV6)) )
         return(TRAD_SUCCESS);
      switch(i = ((value)) ? tinyrad_strtobool(value) : TRAD_YES)
      {  case TRAD_NO:  tinyrad_set_option(tr, TRAD_OPT_IPV6, &i); break;
         case TRAD_YES: tinyrad_set_option(tr, TRAD_OPT_IPV6, &i); break;
         default: break;
      };
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

      case TRAD_CONF_RANDOM:
      TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, TRAD_CONF_RANDOM, \"%s\" )", __func__, (((value)) ? value : "(null)"));
      if ( (!(tr)) || (!(value)) || ((tr->opts & TRAD_RANDOM_MASK)) )
         return(TRAD_SUCCESS);
      else if (!(strcasecmp(value, "rand")))    i = TRAD_RAND;
      else if (!(strcasecmp(value, "random")))  i = TRAD_RANDOM;
      else if (!(strcasecmp(value, "urandom"))) i = TRAD_URANDOM;
      else return(TRAD_SUCCESS);
      return(tinyrad_set_option(tr, TRAD_OPT_RANDOM, &i));

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
         unsigned                      flag,
         const char *                  value )
{
   int      boolean;

   TinyRadDebugTrace();

   if ( (!(tr)) && (!(dict)) )
      return(TRAD_SUCCESS);

   if ((boolean = ((value)) ? tinyrad_strtobool(value) : TRAD_YES) == -1)
      return(TRAD_SUCCESS);

   TinyRadDebug(TRAD_DEBUG_ARGS, "   == %s( tr, %u, \"%s\" )", __func__, flag, ((boolean == TRAD_YES) ? "TRAD_YES" : "TRAD_NO"));

   if ( ((dict)) && ( ((dict->opts | dict->opts_neg) & flag) == 0) )
   {
      switch(boolean)
      {
         case TRAD_YES: dict->opts     |= flag; break;
         case TRAD_NO:  dict->opts_neg |= flag; break;
         default:                               break;
      };
   };

   if ( ((tr)) && ( ((tr->opts | tr->opts_neg) & flag) == 0) )
   {
      switch(boolean)
      {
         case TRAD_YES: tr->opts     |= flag; break;
         case TRAD_NO:  tr->opts_neg |= flag; break;
         default:                             break;
      };
   };

   return(TRAD_SUCCESS);
}


void
tinyrad_conf_print(
         TinyRad *                     tr,
         TinyRadDict *                 dict )
{
   size_t      pos;
   char *      str;
   char        buff[TRAD_LINE_MAX_LEN];

   if ((tr))
   {
      printf("# TinyRad Library Configuration:\n");
      if ((tinyrad_get_option(tr, TRAD_OPT_URI, &str)) == TRAD_SUCCESS)
      {
         printf("%-20s '%s'\n", "URI", str);
         tinyrad_free(str);
      };
      if ((tinyrad_get_option(tr, TRAD_OPT_SOCKET_BIND_ADDRESSES, &str)) == TRAD_SUCCESS)
      {
         printf("%-20s '%s'\n", "BIND_ADDRESS:", str);
         tinyrad_free(str);
      };
      if ( (!(tr->opts & TRAD_IPV4)) && (!(tr->opts & TRAD_IPV6)) )
      {
         tinyrad_conf_print_bool( 1,                        "IPV4",           TRAD_YES );
         tinyrad_conf_print_bool( 1,                        "IPV6",           TRAD_YES );
      } else {
         tinyrad_conf_print_bool( 0,                        "IPV4",           ((tr->opts & TRAD_IPV4)) );
         tinyrad_conf_print_bool( 0,                        "IPV6",           ((tr->opts & TRAD_IPV6)) );
      };
      tinyrad_conf_print_str(  0,                           "SECRET_FILE",    tr->secret_file);
      tinyrad_conf_print_str(  (tr->secret_file != NULL),   "SECRET",         tr->secret);
      tinyrad_conf_print_int(  0,                           "NETWORK_TIMEOUT", (int)(((tr->net_timeout)) ? tr->net_timeout->tv_sec : 0));
      tinyrad_conf_print_int(  0,                           "TIMEOUT",         (int)tr->timeout);
      tinyrad_conf_print_hex(  1,                           "authenticator",   tr->authenticator);
      switch(tr->opts & TRAD_RANDOM_MASK)
      {  case TRAD_RAND:    tinyrad_conf_print_line(  0, "RANDOM", "rand");    break;
         case TRAD_RANDOM:  tinyrad_conf_print_line(  0, "RANDOM", "random");  break;
         case TRAD_URANDOM: tinyrad_conf_print_line(  0, "RANDOM", "urandom"); break;
         default:           tinyrad_conf_print_int(   0, "RANDOM", (tr->opts | TRAD_RANDOM_MASK)); break;
      };
      printf("\n");
   };

   if ((dict))
   {
      printf("# TinyRad Dictionary Configuration:\n");
      tinyrad_conf_print_bool( 0,                        "BUILTIN_DICTIONARY",   ((dict->opts & TRAD_BUILTIN_DICT)) );
      tinyrad_conf_print_str(  (dict->default_dictfile == NULL),  "DICTIONARY",   dict->default_dictfile );
      tinyrad_conf_print_bool( 1,                        "IPV4",           TRAD_YES );
      if ( ((dict->paths)) && ((dict->paths[0])) )
      {
         buff[0] = '\0';
         for(pos = 0; ((dict->paths[pos])); pos++)
         {
            if (!(dict->paths[pos][0]))
               continue;
            if ((buff[0]))
               tinyrad_strlcat(buff, ":", sizeof(buff));
            tinyrad_strlcat(buff, dict->paths[pos], sizeof(buff));
         };
         tinyrad_conf_print_str( 0,  "PATHS", buff );
      };
      printf("\n");
   };

   printf("STOPINIT\n\n");

   return;
}


void
tinyrad_conf_print_bool(
         int                           comment,
         const char *                  name,
         int                           value)
{
   tinyrad_conf_print_line(comment, name, (((value)) ? "yes" : "no"));
   return;
}


void
tinyrad_conf_print_hex(
         int                           comment,
         const char *                  name,
         unsigned                      value )
{
   char buff[9];
   snprintf(buff, sizeof(buff), "%08x", value);;
   tinyrad_conf_print_line(comment, name, buff);
   return;
}


void
tinyrad_conf_print_int(
         int                           comment,
         const char *                  name,
         int                           value )
{
   char buff[32];
   snprintf(buff, sizeof(buff), "%i", value);;
   tinyrad_conf_print_line(comment, name, buff);
   return;
}


void
tinyrad_conf_print_line(
         int                           comment,
         const char *                  name,
         const char *                  value )
{
   printf("%s%-20s %s\n", (((comment)) ? "#" : ""), name, value);
   return;
}


void
tinyrad_conf_print_str(
         int                           comment,
         const char *                  name,
         const char *                  value )
{
   char buff[TRAD_LINE_MAX_LEN];
   if (!(value))
      return;
   snprintf(buff, sizeof(buff), "'%s'", value);
   tinyrad_conf_print_line(comment, name, buff);
   return;
}


/* end of source */
