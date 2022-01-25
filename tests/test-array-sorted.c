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
#define _TESTS_TINYRAD_ARRAY_SORTED_C 1


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
#include <assert.h>
#include <inttypes.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tinyrad.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-array-sorted"

#define MY_LIST_LEN  256

#define MY_OBJ_NAME     0x01000000
#define MY_OBJ_VALUE    0x02000000
#define MY_KEY_NAME     0x04000000
#define MY_KEY_VALUE    0x08000000
#define MY_MASK         0x0f000000


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#pragma mark - Data Types

typedef struct _mydata MyData;

struct _mydata
{
   char *            name;
   int               value;
   int               opts;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

static const char * test_strs[] =
{
   "cccccccc",
   "bbbbbbbb",
   "aaaaaaaa",
   NULL
};


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


int
my_compare_key_name(
         const void *                  a,
         const void *                  b );


int
my_compare_key_value(
         const void *                  a,
         const void *                  b );


int
my_compare_obj_name(
         const void *                  a,
         const void *                  b );


int
my_compare_obj_value(
         const void *                  a,
         const void *                  b );


void
my_compar_opts(
         int                           opts,
         const char **                 compar_namep,
         int (**comparp)(const void *, const void *) );


int
my_test_insert(
         int                           opts,
         MyData ***                    listp,
         size_t *                      list_lenp,
         MyData **                     src,
         MyData **                     test,
         size_t                        dat_len,
         unsigned                      arrayopt );


int
my_test_remove(
         int                           opts,
         MyData **                     list,
         size_t *                      list_lenp,
         MyData **                     src,
         MyData **                     test,
         size_t                        dat_len,
         unsigned                      arrayopt );


int
my_test_search(
         int                           opts,
         MyData **                     list,
         size_t                        len );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

int main( int argc, char * argv[] )
{
   int                  opt;
   int                  c;
   int                  opt_index;
   int                  opts;
   unsigned             merge;
   size_t               pos;
   size_t               len;
   size_t               list_len;
   MyData               data[MY_LIST_LEN];
   MyData *             src[MY_LIST_LEN];
   MyData *             test[MY_LIST_LEN];
   MyData **            list;

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

   our_initialize(PROGRAM_NAME);

   opts = 0;

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
         break;

         case 'V':
         printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
         return(0);

         case 'v':
         opts |= TRAD_TEST_VERBOSE;
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

   if (optind != argc)
   {
         fprintf(stderr, "%s: unknown argument `--%s'\n", PROGRAM_NAME, argv[optind]);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
   };


   // initialize test data
   our_verbose(opts, "initializing test data ...");
   memset(data, 0, sizeof(data));
   memset(test, 0, sizeof(test));
   for(len = 0; ((test_strs[len])); len++);
   for(pos = 0; (pos < MY_LIST_LEN); pos++)
   {
      data[pos].value = (int)(~pos);
      data[pos].opts  = (int)pos;
      data[pos].name  = (pos < len) ? strdup(test_strs[pos]) : our_random_str(8, 8);
      if (!(data[pos].name))
      {
         for(; (pos > 0); pos--)
            free(data[pos-1].name);
         return(our_error(opts, "out of virtual memory"));
      };
   };


   // populate src and test arrays
   for(pos = 0; (pos < MY_LIST_LEN); pos++)
      test[pos] = src[pos] = &data[pos];


   // test searching by value
   our_verbose(opts, "sorting test data by value ...");
   qsort(test, MY_LIST_LEN, sizeof(MyData *), &my_compare_obj_value);
   opts = opts & ~MY_MASK;
   if ((my_test_search(opts|MY_OBJ_VALUE, test, MY_LIST_LEN)))
      return(1);
   if ((my_test_search(opts|MY_KEY_VALUE, test, MY_LIST_LEN)))
      return(1);


   // test searching by name
   our_verbose(opts, "sorting test data by name ...");
   qsort(test, MY_LIST_LEN, sizeof(MyData *), &my_compare_obj_name);
   opts = opts & ~MY_MASK;
   if ((my_test_search(opts|MY_OBJ_NAME, test, MY_LIST_LEN)))
      return(1);
   if ((my_test_search(opts|MY_KEY_NAME, test, MY_LIST_LEN)))
      return(1);


   // insert data as sorted list using TINYRAD_ARRAY_REPLACE
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_NAME;
   merge    = TINYRAD_ARRAY_REPLACE;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


   // insert data as sorted list using TINYRAD_ARRAY_INSERT
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_NAME;
   merge    = TINYRAD_ARRAY_INSERT;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_UNORDERED
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_NAME;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_UNORDERED;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_APPEND
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_NAME;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_APPEND;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


  // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_PREPEND
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_NAME;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_PREPEND;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


   // test searching by value
   our_verbose(opts, "sorting test data by value ...");
   qsort(test, MY_LIST_LEN, sizeof(MyData *), &my_compare_obj_value);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_UNORDERED
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_VALUE;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_UNORDERED;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_APPEND
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_VALUE;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_APPEND;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);


  // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_PREPEND
   our_verbose(opts, "resetting list ...");
   list     = NULL;
   list_len = 0;
   opts     = (opts & ~MY_MASK) | MY_OBJ_VALUE;
   merge    = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_PREPEND;
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_insert(opts, &list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((my_test_remove(opts, list, &list_len, src, test, MY_LIST_LEN, merge)))
      return(1);
   if ((list_len))
      return(our_error(opts, "list still has elements"));
   free(list);

   return(0);
}


int my_compare_key_name(const void * a, const void * b)
{
   MyData * const *  obj;
   const char *      key;
   obj = a;
   key = b;
   return(strcasecmp((*obj)->name, key));
}


int my_compare_key_value(const void * a, const void *b)
{
   MyData * const *  x;
   const int *       y;
   x = a;
   y = b;
   return((*x)->value - *y);
}


int my_compare_obj_name(const void * a, const void *b)
{
   const MyData * const * x;
   const MyData * const * y;
   x = a;
   y = b;
   return(strcasecmp((*x)->name, (*y)->name));
}


int my_compare_obj_value(const void * a, const void *b)
{
   const MyData * const * x;
   const MyData * const * y;
   x = a;
   y = b;
   return((*x)->value - (*y)->value);
}


void
my_compar_opts(
         int                           opts,
         const char **                 compar_namep,
          int (**comparp)(const void *, const void *) )
{
   switch(opts & MY_MASK)
   {
      case MY_KEY_NAME:
      *compar_namep = "my_compare_key_name";
      *comparp      = &my_compare_key_name;
      break;

      case MY_KEY_VALUE:
      *compar_namep = "my_compare_key_value";
      *comparp      = &my_compare_key_value;
      break;

      case MY_OBJ_NAME:
      *compar_namep = "my_compare_obj_name";
      *comparp      = &my_compare_obj_name;
      break;

      case MY_OBJ_VALUE:
      *compar_namep = "my_compare_obj_value";
      *comparp      = &my_compare_obj_value;
      break;

      default:
      *compar_namep = NULL;
      *comparp      = NULL;
      break;
   };
   assert(*compar_namep != NULL);
   return;
}


int
my_test_insert(
         int                           opts,
         MyData ***                    listp,
         size_t *                      list_lenp,
         MyData **                     src,
         MyData **                     test,
         size_t                        dat_len,
         unsigned                      arrayopt )
{
   size_t         x;
   size_t         pos;
   size_t         len;
   size_t         iteration;
   unsigned       mergeopt;
   const char *   compar_name;
   const char *   action_name;
   const char *   merge_type;
   MyData **      list;
   int (*compar)(const void *, const void *);

   assert(listp       != NULL);
   assert(list_lenp   != NULL);
   assert(src         != NULL);
   assert(test        != NULL);
   assert(dat_len      > 0);

   my_compar_opts(opts, &compar_name, &compar);

   mergeopt = (arrayopt & TINYRAD_ARRAY_MASK_MERGE);
   iteration = *list_lenp / dat_len;
   switch(arrayopt & TINYRAD_ARRAY_MASK_INSERT)
   {
      case TINYRAD_ARRAY_INSERT:  action_name = "INSERT"; break;
      case TINYRAD_ARRAY_MERGE:   action_name = "MERGE"; break;
      case TINYRAD_ARRAY_REPLACE: action_name = "REPLACE"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };
   switch(mergeopt)
   {
      case TINYRAD_ARRAY_PREPEND:   merge_type = "PREPEND"; break;
      case TINYRAD_ARRAY_APPEND:    merge_type = "APPEND"; break;
      case TINYRAD_ARRAY_UNORDERED: merge_type = "UNORDERED"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };

   our_verbose(opts, "testing   tinyrad_array_insert( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   for(x = 0; (x < dat_len); x++)
   {
      if (tinyrad_array_insert((void **)listp, list_lenp, sizeof(MyData *), &src[x], arrayopt, compar, NULL, &realloc) == -1)
         return(our_error(opts, "tinyrad_array_insert(%s): returned error", action_name));
      len = (iteration*dat_len) + x + 1;
      if ( ((iteration)) && (!(arrayopt & TINYRAD_ARRAY_MERGE)) )
         len = dat_len;
      if (*list_lenp != len)
         return(our_error(opts, "tinyrad_array_insert(%s): did not increment length", action_name));
   };

   list = *listp;

   if (!(arrayopt & TINYRAD_ARRAY_MERGE))
      iteration = 0;

   our_verbose(opts, "verifying tinyrad_array_insert( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   iteration++;
   for(x = 0; (x < (dat_len*iteration)); x++)
   {
      pos = x/iteration;
      if ((strcasecmp(test[pos]->name, list[x]->name)))
         return(our_error(opts, "tinyrad_array_insert(%s): misordered the list", action_name));
      switch(mergeopt)
      {
         case TINYRAD_ARRAY_APPEND:
         if ( (x < ((dat_len*iteration)-1)) && ((x % iteration) == (iteration-1)) )
            if (!(strcasecmp(list[x]->name, list[x+1]->name)))
               return(our_error(opts, "tinyrad_array_insert(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_PREPEND:
         if ( (x > 0) && (!(x % iteration)) )
            if (!(strcasecmp(list[x]->name, list[x-1]->name)))
               return(our_error(opts, "tinyrad_array_insert(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_UNORDERED:
         default:
         break;
      };
   };

   return(0);
}


int
my_test_remove(
         int                           opts,
         MyData **                     list,
         size_t *                      list_lenp,
         MyData **                     src,
         MyData **                     test,
         size_t                        dat_len,
         unsigned                      arrayopt )
{
   size_t         x;
   size_t         pos;
   size_t         len;
   size_t         iteration;
   unsigned       mergeopt;
   const char *   compar_name;
   const char *   action_name;
   const char *   merge_type;
   int (*compar)(const void *, const void *);

   assert(list        != NULL);
   assert(list_lenp   != NULL);
   assert(test        != NULL);
   assert(dat_len      > 0);

   my_compar_opts(opts, &compar_name, &compar);

   mergeopt = (arrayopt & TINYRAD_ARRAY_MASK_MERGE);
   iteration = (*list_lenp / dat_len) - 1;
   switch(arrayopt & TINYRAD_ARRAY_MASK_INSERT)
   {
      case TINYRAD_ARRAY_INSERT:  action_name = "INSERT"; break;
      case TINYRAD_ARRAY_MERGE:   action_name = "MERGE"; break;
      case TINYRAD_ARRAY_REPLACE: action_name = "REPLACE"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };
   switch(mergeopt)
   {
      case TINYRAD_ARRAY_PREPEND:   merge_type = "PREPEND"; break;
      case TINYRAD_ARRAY_APPEND:    merge_type = "APPEND"; break;
      case TINYRAD_ARRAY_UNORDERED: merge_type = "UNORDERED"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };

   our_verbose(opts, "testing   tinyrad_array_remove( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   for(x = 0; (x < dat_len); x++)
   {
      if (tinyrad_array_remove(list, list_lenp, sizeof(MyData *), &src[x], arrayopt, compar, NULL) == -1)
         return(our_error(opts, "tinyrad_array_remove(%s): returned error", action_name));
      len = ((iteration+1)*dat_len) - x - 1;
      if ( ((iteration)) && (!(arrayopt & TINYRAD_ARRAY_MERGE)) )
         len = dat_len;
      if (*list_lenp != len)
         return(our_error(opts, "tinyrad_array_remove(%s): did not decrement length", action_name));
   };

   if (!(arrayopt & TINYRAD_ARRAY_MERGE))
      iteration = 0;

   our_verbose(opts, "verifying tinyrad_array_remove( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   for(x = 0; ((x < (*list_lenp)) && ((iteration))); x++)
   {
      pos = x/iteration;
      if ((strcasecmp(test[pos]->name, list[x]->name)))
         return(our_error(opts, "tinyrad_array_remove(%s): misordered the list", action_name));
      switch(mergeopt)
      {
         case TINYRAD_ARRAY_APPEND:
         if ( (x < ((dat_len*iteration)-1)) && ((x % iteration) == (iteration-1)) )
            if (!(strcasecmp(list[x]->name, list[x+1]->name)))
               return(our_error(opts, "tinyrad_array_remove(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_PREPEND:
         if ( (x > 0) && (!(x % iteration)) )
            if (!(strcasecmp(list[x]->name, list[x-1]->name)))
               return(our_error(opts, "tinyrad_array_remove(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_UNORDERED:
         default:
         break;
      };
   };

   return(0);
}


int
my_test_search(
         int                           opts,
         MyData **                     list,
         size_t                        len )
{
   size_t         x;
   size_t         y;
   ssize_t        idx;
   void *         ptr;
   MyData **      res;
   const char *   compar_name;
   int (*compar)(const void *, const void *);

   assert(list != NULL);

   my_compar_opts(opts, &compar_name, &compar);

   our_verbose(opts, "testing   tinyrad_array_search( %s ) ...", compar_name);
   for(x = 1; (x < (len+1)); x++)
   {
      for(y = 0; (y < x); y++)
      {
         switch(opts & MY_MASK)
         {
            case MY_OBJ_NAME:  ptr = &list[y];        break;
            case MY_OBJ_VALUE: ptr = &list[y];        break;
            case MY_KEY_NAME:  ptr = list[y]->name;   break;
            case MY_KEY_VALUE: ptr = &list[y]->value; break;
            default:
            return(our_error(opts, "unknown key"));
         };
         if ((idx = tinyrad_array_search(list, x, sizeof(MyData *), ptr, TINYRAD_ARRAY_FIRST, NULL, compar)) == -1)
            return(our_error(opts, "tinyrad_array_search(); size: %zu; func: %s; idx: %zu; search error", x, compar_name, y));
         if ((size_t)idx != y)
            return(our_error(opts, "tinyrad_array_search(); size: %zu; func: %s; idx: %zu; index mismatch", x, compar_name, y));
      };
   };

   our_verbose(opts, "testing   tinyrad_array_get(    %s ) ...", compar_name);
   for(x = 1; (x < (len+1)); x++)
   {
      for(y = 0; (y < x); y++)
      {
         switch(opts & MY_MASK)
         {
            case MY_OBJ_NAME:  ptr = &list[y];        break;
            case MY_OBJ_VALUE: ptr = &list[y];        break;
            case MY_KEY_NAME:  ptr = list[y]->name;   break;
            case MY_KEY_VALUE: ptr = &list[y]->value; break;
            default:
            return(our_error(opts, "unknown key"));
         };
         if ((res = tinyrad_array_get(list, x, sizeof(MyData *), ptr, TINYRAD_ARRAY_FIRST, compar)) == NULL)
            return(our_error(opts, "tinyrad_array_get(); size: %zu; func: %s; idx: %zu; search error", x, compar_name, y));
         if ((strcasecmp((*res)->name, list[y]->name)))
            return(our_error(opts, "tinyrad_array_get(); size: %zu; func: %s; idx: %zu; result name does not match", x, compar_name, y));
         if ((*res)->value != list[y]->value)
            return(our_error(opts, "tinyrad_array_get(); size: %zu; func: %s; idx: %zu; result value does not match", x, compar_name, y));
      };
   };

   return(0);
}

/* end of source */

