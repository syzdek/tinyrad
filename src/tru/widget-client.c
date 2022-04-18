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
#define _SRC_TRU_WIDGET_CLIENT_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include "tinyradutil.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct tru_client_state
{
   int                     fd;
   int                     padint;
   char **                 pairs;
   TinyRadAttrList *       attrlist;
   char                    line[TRAD_LINE_MAX_LEN];
} TRUClientState;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes

static const TinyRadPktType *
tru_widget_client_pkttype(
         const char *                  pname );


static int
tru_widget_client_process_argv(
         TinyRadUtilConf *             cnf,
         const TinyRadPktType *        pkttype,
         TRUClientState *              state );


static int
tru_widget_client_process_file(
         TinyRadUtilConf *             cnf,
         const TinyRadPktType *        pkttype,
         TRUClientState *              state );


static int
tru_widget_client_req_add(
         TinyRadUtilConf *             cnf,
         TRUClientState *              state,
         char *                        pair );


static void
tru_widget_client_state_cleanup(
         TRUClientState *              state );


static void
tru_widget_client_state_reset(
         TRUClientState *              state );


static int
tru_widget_client_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark tru_pkttype_map[]
// https://www.iana.org/assignments/radius-types/
static TinyRadPktType tru_pkttype_map[] =
{
   { .type =   0, .name = "unknown",               .alias = NULL,          .desc = "unknown packet type" },
   { .type =   1, .name = "access-request",        .alias = "auth",        .desc = "Access-Request packet (RFC2865)" },
   { .type =   2, .name = "access-accept",         .alias = NULL,          .desc = "Access-Accept packet (RFC2865)" },
   { .type =   3, .name = "access-reject",         .alias = NULL,          .desc = "Access-Reject packet (RFC2865)" },
   { .type =   4, .name = "accounting-request",    .alias = "acct",        .desc = "Accounting-Request packet (RFC2866)", },
   { .type =   5, .name = "accounting-response",   .alias = NULL,          .desc = "Accounting-Response packet (RFC2866)", },
   { .type =   6, .name = "accounting-interim",    .alias = "interim",     .desc = "Accounting-Status packet (RFC2882, RFC3575)" },
   { .type =   6, .name = "accounting-status",     .alias = NULL,          .desc = "Accounting-Status packet (RFC2882, RFC3575)" },
   { .type =  40, .name = "disconnect-request",    .alias = "disconnect",  .desc = "Disconnect-Request packet (RFC3575, RFC5176)" },
   { .type =  41, .name = "disconnect-ack",        .alias = NULL,          .desc = "Disconnect-ACK packet (RFC3575, RFC5176)" },
   { .type =  42, .name = "disconnect-nak",        .alias = NULL,          .desc = "Disconnect-NAK packet (RFC3575, RFC5176)" },
   { .type =  43, .name = "coa-request",           .alias = "coa",         .desc = "CoA-Request packet (RFC3575, RFC5176)" },
   { .type =  44, .name = "coa-ack",               .alias = NULL,          .desc = "CoA-ACK packet (RFC3575, RFC5176)" },
   { .type =  45, .name = "coa-nak",               .alias = NULL,          .desc = "CoA-NAK packet (RFC3575, RFC5176)" },
   { .type = 256, .name = NULL, .alias = NULL, .desc = NULL }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int
tru_widget_client(
         TinyRadUtilConf *                 cnf )
{
   int                        rc;
   const TinyRadPktType *     pkttype;
   TRUClientState             state;

   // getopt options
   static char          short_opt[] = TRU_ALL_SHORT;
   static struct option long_opt[]  = { TRU_ALL_LONG };

   memset(&state, 0, sizeof(state));
   state.fd = -1;

   // process widget cli options
   if ((rc = tru_cli_parse(cnf, cnf->argc, cnf->argv, short_opt, long_opt, &tru_widget_client_usage)) != 0)
      return((rc == TRU_GETOPT_EXIT) ? 0 : 1);
   if (optind >= cnf->argc)
   {
      fprintf(stderr, "%s: %s: missing required argument\n", PROGRAM_NAME, cnf->widget_name);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
      return(1);
   };
   if ( ((optind+1) < cnf->argc) && ((cnf->attr_file)) )
   {
      fprintf(stderr, "%s: %s: -f is incompatible with attribute arguments\n", PROGRAM_NAME, cnf->widget_name);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
      return(1);
   };
   if ((pkttype = tru_widget_client_pkttype(cnf->argv[optind])) == NULL)
   {
      fprintf(stderr, "%s: %s: unknown or ambiguous packet type\n", PROGRAM_NAME, cnf->widget_name);
      fprintf(stderr, "Try `%s %s --help' for more information.\n", PROGRAM_NAME, cnf->widget_name);
      return(1);
   };
   optind++;

   // initialize TinyRad library
   if ((rc = tru_load_tinyrad(cnf)) != TRAD_SUCCESS)
      return(rc);

   // initialize attribute list
   if ((rc = tinyrad_attr_list_initialize(cnf->tr, &state.attrlist)) != TRAD_SUCCESS)
      return(rc);

   // process arguments as attributes
   rc = (optind < cnf->argc)
      ? tru_widget_client_process_argv(cnf, pkttype, &state)
      : tru_widget_client_process_file(cnf, pkttype, &state);

   tru_widget_client_state_cleanup(&state);

   return(rc);
}


const TinyRadPktType *
tru_widget_client_pkttype(
         const char *                  pname )
{
   size_t                     pos;
   size_t                     pname_len;
   size_t                     matches;
   intptr_t                   code;
   char *                     endptr;
   const TinyRadPktType *     match;
   const TinyRadPktType *     pkttype;

   if ( (!(pname)) || (!(pname[0])) )
      return(NULL);

   // attempt to lookup packet type by code
   code = strtoll(pname, &endptr, 0);
   if ( (endptr != pname) && (endptr[0] == '\0') )
   {
      if ( (code > 255) || ( code < 0) )
         return(NULL);
      for(pos = 0; ((tru_pkttype_map[pos].name)); pos++)
         if (tru_pkttype_map[pos].type == (unsigned)code)
            return(&tru_pkttype_map[pos]);
      tru_pkttype_map[0].type = (unsigned)code;
      return(&tru_pkttype_map[0]);
   };

   match       = NULL;
   matches     = 0;
   pname_len   = strlen(pname);

   // attempt to lookup packet type by name and alias
   for(pos = 0; ((tru_pkttype_map[pos].name)); pos++)
   {
      pkttype = &tru_pkttype_map[pos];

      // check packet name
      if (!(strncmp(pkttype->name, pname, pname_len)))
      {
         if (pkttype->name[pname_len] == '\0')
            return(pkttype);
         match = pkttype;
         matches++;
      };

      // check packet alias
      if (!(pkttype->alias))
         continue;
      if (!(strncmp(pkttype->alias, pname, pname_len)))
      {
         if (pkttype->alias[pname_len] == '\0')
            return(pkttype);
         match = pkttype;
         matches++;
      };
   };

   return( (matches > 1) ? NULL : match );
}


int
tru_widget_client_process_argv(
         TinyRadUtilConf *             cnf,
         const TinyRadPktType *        pkttype,
         TRUClientState *              state )
{
   int                  pos;

   assert(pkttype != NULL);
   assert(state   != NULL);

   for(pos = optind; (pos < cnf->argc); pos++)
      printf("arg %i: %s\n", pos, cnf->argv[pos]);

   return(0);
}


int
tru_widget_client_process_file(
         TinyRadUtilConf *             cnf,
         const TinyRadPktType *        pkttype,
         TRUClientState *              state )
{
   int                  rc;
   size_t               len;
   size_t               lineno;
   int                  x;

   assert(pkttype != NULL);
   assert(state   != NULL);

   // open file
   state->fd = STDIN_FILENO;
   if ( ((cnf->attr_file)) && ((strcmp("-", cnf->attr_file))) )
      if ((state->fd = open(cnf->attr_file, O_RDONLY)) == -1)
         return(tru_error(cnf, TRAD_EUNKNOWN, "open(): %s: %s", cnf->attr_file, strerror(errno)));
   if (state->fd == STDIN_FILENO)
      cnf->attr_file = "stdin";

   // loop through file
   for(lineno = len = 1; (len > 0); lineno++)
   {
      // read next line
      if ((rc = tinyrad_readline(state->fd, state->line, (sizeof(state->line)-1), &len)) != TRAD_SUCCESS)
         return(tru_error(cnf, rc, "tinyrad_readline(): %s", tinyrad_strerror(rc)));
      state->line[len] = '\0';

      // exit if single '.' on line
      if ( (state->fd == STDIN_FILENO) && (!(strcmp(".", state->line))) )
      {
         return(TRAD_SUCCESS);
      };

      // submit request if empty line
      if (!(state->line[0]))
      {
         if (!(state->attrlist))
            continue;
printf("sending request\n");
         tru_widget_client_state_reset(state);
         continue;
      };

      // split line into attribute/value pairs
      tinyrad_strtrim(state->line);
printf("%zu: %s\n", lineno, state->line);
      if ((rc = tinyrad_strsplit(state->line, ',', &state->pairs, NULL)) != TRAD_SUCCESS)
         return(tru_error(cnf, rc, NULL));

      // loop through pairs and add to attribute list
      for(x = 0; ((state->pairs[x])); x++)
         if ((rc = tru_widget_client_req_add(cnf, state, state->pairs[x])) != TRAD_SUCCESS)
            return(tru_error(cnf, rc, "%s:%i: %s", cnf->attr_file, lineno, tinyrad_strerror(rc)));
   };

   return(TRAD_SUCCESS);
}


int
tru_widget_client_req_add(
         TinyRadUtilConf *             cnf,
         TRUClientState *              state,
         char *                        pair )
{
   int            rc;
   int            argc;
   char **        argv;

   assert(cnf   != NULL);
   assert(state != NULL);
   assert(pair  != NULL);
printf("   pair: %s\n", pair);

   tinyrad_strtrim(pair);
   if (!(pair[0]))
      return(TRAD_SUCCESS);

   // initialize attribute list
   if (!(state->attrlist))
      if ((rc = tinyrad_attr_list_initialize(cnf->tr, &state->attrlist)) != TRAD_SUCCESS)
         return(tru_error(cnf, rc, "%s", tinyrad_strerror(rc)));

   // split attribute and value
   if ((rc = tinyrad_strsplit(pair, '=', &argv, &argc)) != TRAD_SUCCESS)
      return(rc);
   if (argc != 2)
   {
      tinyrad_strsfree(argv);
      return(TRAD_ESYNTAX);
   };

   // clean up strings
   tinyrad_strtrim(argv[0]);
   tinyrad_strtrim(argv[1]);
   if ( (!(argv[0][0])) || (!(argv[1][0])) )
   {
      tinyrad_strsfree(argv);
      return(TRAD_ESYNTAX);
   };

   return(TRAD_SUCCESS);
}


void
tru_widget_client_state_cleanup(
         TRUClientState *              state )
{
   if (!(state))
      return;
   tru_widget_client_state_reset(state);
   if ( (state->fd != -1) && (state->fd != STDIN_FILENO) )
      close(state->fd);
   state->fd = -1;
   return;
}


void
tru_widget_client_state_reset(
         TRUClientState *              state )
{
   if (!(state))
      return;

   if ((state->attrlist))
      tinyrad_free(state->attrlist);
   state->attrlist = NULL;

   if ((state->pairs))
      tinyrad_strsfree(state->pairs);
   state->pairs = NULL;

   state->line[0] = '\0';

   return;
}


int
tru_widget_client_usage(
         TinyRadUtilConf *             cnf,
         const char *                  short_opt )
{
   size_t                     pos;
   const TinyRadPktType *     type;

   tru_usage_summary(cnf);
   tru_usage_options(short_opt);
   printf("\n");
   printf("REQUEST TYPES:\n");
   printf("  1-255                     numeric packet type code\n");
   for(pos = 0; ((tru_pkttype_map[pos].name)); pos++)
   {
      type = &tru_pkttype_map[pos];
      if (!(type->alias))
         continue;
      printf("  %-25s %s\n", type->alias, type->desc);
   };
   printf("\n");

   return(0);
}


/* end of source */
