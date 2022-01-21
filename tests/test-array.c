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
#define _TESTS_TINYRAD_ARRAY_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tinyrad.h>

#include "common.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions

#undef PROGRAM_NAME
#define PROGRAM_NAME "tinyrad-array"

#define MY_LIST_LEN  256

#define MY_KEY_NONE  0
#define MY_KEY_NAME  1
#define MY_KEY_VALUE 2


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

int main(
         int                           argc,
         char *                        argv[] );


int my_compare_key_name(
         const void *                  a,
         const void *                  b );


int my_compare_key_value(
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


int
my_test_insert(
         int                           opts,
         MyData **                     list,
         MyData **                     src,
         size_t                        src_len,
         size_t                        iteration,
         unsigned                      action,
         const char *                  compar_name,
         int (*compar)(const void *, const void *) );


int
my_test_search(
         int                           opts,
         MyData **                     list,
         size_t                        list_len,
         int                           keyfld,
         const char *                  compar_name,
         int (*compar)(const void *, const void *) );


char *
my_random_str(
         size_t                        min,
         size_t                        max );


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
   unsigned             seed;
   unsigned             merge;
   size_t               pos;
   size_t               len;
   struct timespec      ts;
   MyData               data[MY_LIST_LEN];
   MyData *             test[MY_LIST_LEN];
   MyData *             list[MY_LIST_LEN*3];

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

   opt = TRAD_OFF;
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_SYSLOG, &opt);
   tinyrad_set_option(NULL, TRAD_OPT_DEBUG_IDENT, PROGRAM_NAME);

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

   clock_gettime(CLOCK_UPTIME_RAW, &ts);
   seed = (unsigned)(ts.tv_sec + ts.tv_sec);
   srandom(seed);

   // initialize test data
   our_verbose(opts, "initializing test data ...");
   bzero(data, sizeof(data));
   bzero(list, sizeof(list));
   bzero(test, sizeof(test));
   for(len = 0; ((test_strs[len])); len++);
   for(pos = 0; (pos < MY_LIST_LEN); pos++)
   {
      data[pos].value = (int)(~pos);
      data[pos].opts  = (int)pos;
      data[pos].name  = (pos < len) ? strdup(test_strs[pos]) : my_random_str(8, 8);
      if (!(data[pos].name))
      {
         for(; (pos > 0); pos--)
            free(data[pos-1].name);
         return(our_error(opts, "out of virtual memory"));
      };
   };

   // populate test array
   for(pos = 0; (pos < MY_LIST_LEN); pos++)
      test[pos] = &data[pos];


   // sort test data by value
   our_verbose(opts, "sorting test data by value ...");
   qsort(test, MY_LIST_LEN, sizeof(MyData *), &my_compare_obj_value);


   // test searching by value
   if ((my_test_search(opts, test, MY_LIST_LEN, MY_KEY_NONE, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_search(opts, test, MY_LIST_LEN, MY_KEY_VALUE, "my_compare_key_value", &my_compare_key_value)))
      return(1);


   // sort test data by name
   our_verbose(opts, "sorting test data by name ...");
   qsort(test, MY_LIST_LEN, sizeof(MyData *), &my_compare_obj_name);


   // test searching by name
   if ((my_test_search(opts, test, MY_LIST_LEN, MY_KEY_NONE, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_search(opts, test, MY_LIST_LEN, MY_KEY_NAME, "my_compare_key_name", &my_compare_key_name)))
      return(1);


   // insert data as sorted list using TINYRAD_BTREE_REPLACE
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, TINYRAD_ARRAY_REPLACE, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, TINYRAD_ARRAY_REPLACE, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);


   // insert data as sorted list using TINYRAD_BTREE_INSERT
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, TINYRAD_ARRAY_INSERT, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_UNORDERED
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_UNORDERED;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_APPEND
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_APPEND;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);


  // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_PREPEND
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_PREPEND;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_name", &my_compare_obj_name)))
      return(1);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_UNORDERED
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_UNORDERED;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);


   // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_APPEND
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_APPEND;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);


  // insert data as sorted list using TINYRAD_ARRAY_MERGE and TINYRAD_ARRAY_PREPEND
   our_verbose(opts, "clearing list ...");
   bzero(list, sizeof(list));
   merge = TINYRAD_ARRAY_MERGE | TINYRAD_ARRAY_PREPEND;
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 0, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 1, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);
   if ((my_test_insert(opts, list, test, MY_LIST_LEN, 2, merge, "my_compare_obj_value", &my_compare_obj_value)))
      return(1);

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


char * my_random_str(size_t min, size_t max)
{
   char *      str;
   size_t      len;
   size_t      pos;
   static int  count = 0;

   assert(min <= max);

   len = ((max - min)) ? ((unsigned)random() % (max - min)) : 0;
   len += min;
   if (len < sizeof(int))
      len = sizeof(int);

   if ((str = malloc(len+1)) == NULL)
      return(NULL);
   str[len] = '\0';

   for(pos = 0; (pos < (len-sizeof(int))); pos++)
      str[pos] = (random() % 26) + 'a';

   snprintf(&str[pos], (len-pos+1), "%04xu", (~count)&0xffff);

   count++;

   return(str);
}


int
my_test_insert(
         int                           opts,
         MyData **                     list,
         MyData **                     src,
         size_t                        src_len,
         size_t                        iteration,
         unsigned                      action,
         const char *                  compar_name,
         int (*compar)(const void *, const void *) )
{
   size_t         x;
   size_t         pos;
   size_t         len;
   unsigned       mergeopt;
   const char *   action_name;
   const char *   merge_type;

   assert(list        != NULL);
   assert(src         != NULL);
   assert(src_len      > 0);
   assert(iteration    < 4);
   assert(compar_name != NULL);
   assert(compar      != NULL);

   mergeopt = (action & TINYRAD_ARRAY_MASK_MERGE);
   switch(action & TINYRAD_ARRAY_MASK_INSERT)
   {
      case TINYRAD_ARRAY_INSERT:  action_name = "INSERT"; break;
      case TINYRAD_ARRAY_MERGE:   action_name = "MERGE"; break;
      case TINYRAD_ARRAY_REPLACE: action_name = "REPLACE"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };
   switch(action & TINYRAD_ARRAY_MASK_MERGE)
   {
      case TINYRAD_ARRAY_PREPEND:   merge_type = "PREPEND"; break;
      case TINYRAD_ARRAY_APPEND:    merge_type = "APPEND"; break;
      case TINYRAD_ARRAY_UNORDERED: merge_type = "UNORDERED"; break;
      default:
      return(our_error(opts, "unknown insert action"));
   };

   our_verbose(opts, "testing   tinyrad_btree_insert( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   len = (iteration*src_len);
   for(x = 0; (x < src_len); x++)
   {
      len = (iteration*src_len) + x;
      if (tinyrad_array_insert((void **)&list, &len, sizeof(MyData *), &src[x], action, &my_compare_obj_name, NULL, NULL) == -1)
         return(our_error(opts, "tinyrad_btree_insert(%s): returned error", action_name));
      if (len != ((iteration*src_len) + x + 1))
         return(our_error(opts, "tinyrad_btree_insert(%s): did not increment length", action_name));
   };

   our_verbose(opts, "verifying tinyrad_btree_insert( %7s, %9s, %s ) [%s] ...", action_name, merge_type, compar_name, (((iteration)) ? "duplicate" : "unique"));
   iteration++;
   for(x = 0; (x < (src_len*iteration)); x++)
   {
      pos = x/iteration;
      if ((strcasecmp(src[pos]->name, list[x]->name)))
         return(our_error(opts, "tinyrad_btree_insert(%s): misordered the list", action_name));
      switch(mergeopt)
      {
         case TINYRAD_ARRAY_APPEND:
         if ( (x < ((src_len*iteration)-1)) && ((x % iteration) == (iteration-1)) )
            if (!(strcasecmp(list[x]->name, list[x+1]->name)))
               return(our_error(opts, "tinyrad_btree_insert(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_PREPEND:
         if ( (x > 0) && (!(x % iteration)) )
            if (!(strcasecmp(list[x]->name, list[x-1]->name)))
               return(our_error(opts, "tinyrad_btree_insert(%s): first match not returned", action_name));
         break;

         case TINYRAD_ARRAY_UNORDERED:
         default:
         break;
      };
   };

   return(0);
}


int my_test_search(int opts, MyData ** list, size_t len, int keyfld, const char * compar_name, int (*compar)(const void *, const void *))
{
   size_t      x;
   size_t      y;
   ssize_t     idx;
   void *      ptr;
   MyData **   res;

   assert(list != NULL);
   assert(compar != NULL);

   our_verbose(opts, "testing   tinyrad_btree_search( %s ) ...", compar_name);
   for(x = 1; (x < (len+1)); x++)
   {
      for(y = 0; (y < x); y++)
      {
         switch(keyfld)
         {
            case MY_KEY_NONE:  ptr = &list[y];        break;
            case MY_KEY_NAME:  ptr = list[y]->name;   break;
            case MY_KEY_VALUE: ptr = &list[y]->value; break;
            default:
            return(our_error(opts, "unknown key"));
         };
         if ((idx = tinyrad_array_search(list, x, sizeof(MyData *), ptr, TINYRAD_ARRAY_FIRST, NULL, compar)) == -1)
            return(our_error(opts, "tinyrad_btree_search(); size: %zu; func: %s; idx: %zu; search error", x, compar_name, y));
         if ((size_t)idx != y)
            return(our_error(opts, "tinyrad_btree_search(); size: %zu; func: %s; idx: %zu; index mismatch", x, compar_name, y));
      };
   };

   our_verbose(opts, "testing   tinyrad_btree_get(    %s ) ...", compar_name);
   for(x = 1; (x < (len+1)); x++)
   {
      for(y = 0; (y < x); y++)
      {
         switch(keyfld)
         {
            case MY_KEY_NONE:  ptr = &list[y];        break;
            case MY_KEY_NAME:  ptr = list[y]->name;   break;
            case MY_KEY_VALUE: ptr = &list[y]->value; break;
            default:
            return(our_error(opts, "unknown key"));
         };
         if ((res = tinyrad_array_get(list, x, sizeof(MyData *), ptr, TINYRAD_ARRAY_FIRST, compar)) == NULL)
            return(our_error(opts, "tinyrad_btree_get(); size: %zu; func: %s; idx: %zu; search error", x, compar_name, y));
         if ((strcasecmp((*res)->name, list[y]->name)))
            return(our_error(opts, "tinyrad_btree_get(); size: %zu; func: %s; idx: %zu; result name does not match", x, compar_name, y));
         if ((*res)->value != list[y]->value)
            return(our_error(opts, "tinyrad_btree_get(); size: %zu; func: %s; idx: %zu; result value does not match", x, compar_name, y));
      };
   };

   return(0);
}

/* end of source */

