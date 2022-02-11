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
#define _TESTS_DICT_DATA_C 1
#include "dict-data.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#pragma mark - Definitions


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#pragma mark - Variables

#pragma mark test_dict_data_attrs[]
// RADIUS attributes: https://www.iana.org/assignments/radius-types/radius-types.xhtml
const TinyRadDictAttrDef test_dict_data_attrs[] =
{
   { "Syzdek-HTML-Color",         26,      27893,            0,   TRAD_DATATYPE_INTEGER,     0 },  // custom attribute for testing
   { "Syzdek-Faith",              26,      27893,            1,   TRAD_DATATYPE_INTEGER,     0 },  // custom attribute for testing
   { "Syzdek-Animal-Group",       26,      27893,            2,   TRAD_DATATYPE_INTEGER,     0 },  // custom attribute for testing
   { "TinyrRad-Foobar",          224,          0,            0,   TRAD_DATATYPE_TEXT,        0 },  // custom attribute for testing
   { "TinyrRad-Signature",       225,          0,            0,   TRAD_DATATYPE_STRING,      0 },  // custom attribute for testing
   { "TinyrRad-YoYo-Make",       226,          0,            0,   TRAD_DATATYPE_INTEGER,     0 },  // custom attribute for testing
   { NULL, 0, 0, 0, 0, 0 }
};


#pragma mark test_dict_data_values[]
const TinyRadDictValueDef test_dict_data_values[] =
{
   { "Syzdek-Animal-Group",       "Invertebrates",             1 },  // custom value for testing
   { "Syzdek-Animal-Group",       "Fish",                      2 },  // custom value for testing
   { "Syzdek-Animal-Group",       "Amphibians",                3 },  // custom value for testing
   { "Syzdek-Animal-Group",       "Reptiles",                  4 },  // custom value for testing
   { "Syzdek-Animal-Group",       "Birds",                     5 },  // custom value for testing
   { "Syzdek-Animal-Group",       "Mammals",                   6 },  // custom value for testing

   { "Syzdek-HTML-Color",         "Black",                     1 },  // custom value for testing
   { "Syzdek-HTML-Color",         "White",                     2 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Gray",                      3 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Silver",                    4 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Maroon",                    5 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Red",                       6 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Purple",                    7 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Fushsia",                   8 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Green",                     9 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Lime",                     10 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Olive",                    11 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Yellow",                   12 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Navy",                     13 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Blue",                     14 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Teal",                     15 },  // custom value for testing
   { "Syzdek-HTML-Color",         "Aqua",                     16 },  // custom value for testing

   { "Syzdek-Faith",              "Unknown",                   1 },  // custom value for testing
   { "Syzdek-Faith",              "Other",                     2 },  // custom value for testing
   { "Syzdek-Faith",              "Catholic",                  3 },  // custom value for testing
   { "Syzdek-Faith",              "Roman-Rite",                3 },  // custom value for testing
   { "Syzdek-Faith",              "Ritus-Romanus",             3 },  // custom value for testing
   { "Syzdek-Faith",              "Roman-Catholic",            3 },  // custom value for testing
   { "Syzdek-Faith",              "Christian",                 4 },  // custom value for testing
   { "Syzdek-Faith",              "Judaism",                   5 },  // custom value for testing

   { "TinyrRad-YoYo-Make",        "Unknown",                   0 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "YoYoFactory",               1 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "Duncan",                    2 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "iYoYo",                     3 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "YoYoTricks-com",            4 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "Magic-Yoyo",                5 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "Recess",                    6 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "Throwback-Skill-Toys",      7 },  // custom value for testing
   { "TinyrRad-YoYo-Make",        "yoyo-friends",              8 },  // custom value for testing

   { NULL, NULL, 0}
};


#pragma mark test_dict_data_vendors[]
// See https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers)
const TinyRadDictVendorDef test_dict_data_vendors[] =
{
   { "ACS",              8838,   1, 1 },  // ACS Internet, Inc. (https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers)
   { "Syzdek",          27893,   1, 1 },  // DMS  (https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers)
   { "RFC-Example",     32473,   1, 1 },  // Enterprise Number for Documentation Use (RFC 5612)
   { NULL, 0, 0, 0 }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions

void
our_dict_diag_attr(
         unsigned                      opts,
         const TinyRadDictAttrDef *    attr_def,
         TinyRadDictAttr *             attr )
{
   char *      attr_name;
   uint8_t     attr_type;
   uint8_t     attr_type_octets;
   uint8_t     attr_len_octets;
   uint32_t    attr_vendor_id;
   uint32_t    attr_vendor_type;
   uint32_t    attr_data_type;
   uint32_t    attr_flags;
   unsigned    attr_ref_count;

   if ((attr_def))
   {
      our_verbose(opts, "Attribute Definition");
      our_verbose(opts, "   Name:             %s",       attr_def->name);
      our_verbose(opts, "   Type:             %" PRIu8,  attr_def->type);
      our_verbose(opts, "   Vendor ID:        %" PRIu32, attr_def->vendor_id);
      our_verbose(opts, "   Vendor Type:      %" PRIu32, attr_def->vendor_type);
      our_verbose(opts, "   Data Type:        %" PRIu32, attr_def->data_type);
      our_verbose(opts, "   Flags:            0x%08x",   (unsigned)attr_def->flags);
   };

   if ((attr))
   {
      attr_name         = NULL;
      attr_type         = 0;
      attr_vendor_id    = 0;
      attr_vendor_type  = 0;
      attr_data_type    = 0;
      attr_flags        = 0;
      attr_type_octets  = 0;
      attr_len_octets   = 0;
      attr_ref_count    = 0;
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_NAME,         &attr_name);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_TYPE,         &attr_type);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_VEND_ID,      &attr_vendor_id);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_VEND_TYPE,    &attr_vendor_type);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_DATA_TYPE,    &attr_data_type);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_FLAGS,        &attr_flags);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_TYPE_OCTS,    &attr_type_octets);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_LEN_OCTS,     &attr_len_octets);
      tinyrad_dict_attr_info(attr, TRAD_DICT_OPT_REF_COUNT,    &attr_ref_count);
      our_verbose(opts, "Attribute Object");
      our_verbose(opts, "   Name:             %s",       (((attr_name)) ? attr_name : ""));
      our_verbose(opts, "   Type:             %" PRIu8,  attr_type);
      our_verbose(opts, "   Vendor ID:        %" PRIu32, attr_vendor_id);
      our_verbose(opts, "   Vendor Type:      %" PRIu32, attr_vendor_type);
      our_verbose(opts, "   Data Type:        %" PRIu32, attr_data_type);
      our_verbose(opts, "   Flags:            0x%08x",   (unsigned)attr_flags);
      our_verbose(opts, "   Type Octets:      %" PRIu32, attr_type_octets);
      our_verbose(opts, "   Length Octets:    %" PRIu32, attr_len_octets);
      our_verbose(opts, "   Reference Count:  %u",       attr_ref_count);
   };

   return;
}


void
our_dict_diag_value(
         unsigned                      opts,
         const TinyRadDictValueDef *   value_def,
         TinyRadDictValue *            value )
{
   char *      value_name;
   char *      attr_name;
   unsigned    value_ref_count;
   uint8_t     value_type;
   uint32_t    vendor_id;
   uint32_t    vendor_type;
   uint64_t    value_data;

   if ((value_def))
   {
      our_verbose(opts, "Value Definition");
      our_verbose(opts, "   Attribute Name:   %s",       value_def->attr_name);
      our_verbose(opts, "   Value Name:       %s",       value_def->value_name);
      our_verbose(opts, "   Value Data:       %" PRIu64, value_def->data);
   };

   if ((value))
   {
      attr_name            = NULL;
      value_name           = NULL;
      value_type           = 0;
      vendor_id            = 0;
      vendor_type          = 0;
      value_data           = 0;
      value_ref_count      = 0;
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_NAME,         &value_name);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_ATTR_NAME,    &attr_name);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_TYPE,         &value_type);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_VEND_ID,      &vendor_id);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_VEND_TYPE,    &vendor_type);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_DATA,         &value_data);
      tinyrad_dict_value_info(value, TRAD_DICT_OPT_REF_COUNT,    &value_ref_count);
      our_verbose(opts, "Value Object");
      our_verbose(opts, "   Value Name:       %s",         (((value_name)) ? value_name : ""));
      our_verbose(opts, "   Attribute Name:   %s",         (((attr_name))  ? attr_name  : ""));
      our_verbose(opts, "   Attribute Type:   %" PRIu8,    value_type);
      our_verbose(opts, "   Vendor ID:        %" PRIu32,   vendor_id);
      our_verbose(opts, "   Vendor Type:      %" PRIu32,   vendor_type);
      our_verbose(opts, "   Value Data:       %" PRIu64,   value_data);
      our_verbose(opts, "   Reference Count:  %u",         value_ref_count);
   };

   return;
}


void
our_dict_diag_vendor(
         unsigned                      opts,
         const TinyRadDictVendorDef *  def,
         TinyRadDictVendor *           vendor )
{
   char *      vendor_name;
   uint8_t     vendor_type_octets;
   uint8_t     vendor_len_octets;
   uint32_t    vendor_id;
   unsigned    vendor_ref_count;

   if ((def))
   {
      our_verbose(opts, "Vendor Definition");
      our_verbose(opts, "   Name:             %s",       def->name);
      our_verbose(opts, "   Vendor ID:        %" PRIu32, def->vendor_id);
      our_verbose(opts, "   Vendor Type:      %" PRIu32, def->vendor_type_octs);
      our_verbose(opts, "   Data Type:        %" PRIu32, def->vendor_len_octs);
   };

   if ((vendor))
   {
      vendor_name          = NULL;
      vendor_id            = 0;
      vendor_type_octets   = 0;
      vendor_len_octets    = 0;
      vendor_ref_count     = 0;
      tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_NAME,         &vendor_name);
      tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_VEND_ID,      &vendor_id);
      tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_TYPE_OCTS,    &vendor_type_octets);
      tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_LEN_OCTS,     &vendor_len_octets);
      tinyrad_dict_vendor_info(vendor, TRAD_DICT_OPT_REF_COUNT,    &vendor_ref_count);
      our_verbose(opts, "Vendor Object");
      our_verbose(opts, "   Name:             %s",       (((vendor_name)) ? vendor_name : ""));
      our_verbose(opts, "   Vendor ID:        %" PRIu32, vendor_id);
      our_verbose(opts, "   Type Octets:      %" PRIu32, vendor_type_octets);
      our_verbose(opts, "   Length Octets:    %" PRIu32, vendor_len_octets);
      our_verbose(opts, "   Reference Count:  %u",       vendor_ref_count);
   };

   return;
}


int
our_dict_initialize(
         unsigned                      opts,
         TinyRadDict **                dictp )
{
   int            rc;
   char **        errs;

   assert(dictp != NULL);

   *dictp   = NULL;
   errs     = NULL;

   // initialize dictionary
   if (tinyrad_dict_initialize(dictp, 0) != TRAD_SUCCESS)
   {
      our_error(opts, NULL, "out of virtual memory");
      return(1);
   };

   // load builtin/default dictionary objects
   if ((opts & TRAD_TEST_DFLT_DICT))
   {
      our_verbose(opts, "loading default dictionary ...");
      if ((rc = tinyrad_dict_defaults(*dictp, &errs, 0)) != TRAD_SUCCESS)
      {
         our_error(opts, errs, "tinyrad_dict_defaults(): %s", tinyrad_strerror(rc));
         tinyrad_strsfree(errs);
         tinyrad_free(*dictp);
         return(1);
      };
   };

   // load test dictionary vendors
   our_verbose(opts, "adding test dictionary vendors ...");
   if ((rc = tinyrad_dict_import(*dictp, test_dict_data_vendors, NULL, NULL, &errs, 0)) != TRAD_SUCCESS)
   {
      our_error(opts, errs, "tinyrad_dict_import(vendors): %s", tinyrad_strerror(rc));
      tinyrad_strsfree(errs);
      tinyrad_free(*dictp);
      return(1);
   };

   // load test dictionary attributes
   our_verbose(opts, "adding test dictionary attributes ...");
   if ((rc = tinyrad_dict_import(*dictp, NULL, test_dict_data_attrs, NULL, &errs, 0)) != TRAD_SUCCESS)
   {
      our_error(opts, errs, "tinyrad_dict_import(attributes): %s", tinyrad_strerror(rc));
      tinyrad_strsfree(errs);
      tinyrad_free(*dictp);
      return(1);
   };

   // load test dictionary values
   our_verbose(opts, "adding test dictionary values ...");
   if ((rc = tinyrad_dict_import(*dictp, NULL, NULL, test_dict_data_values, &errs, 0)) != TRAD_SUCCESS)
   {
      our_error(opts, errs, "tinyrad_dict_import(values): %s", tinyrad_strerror(rc));
      tinyrad_strsfree(errs);
      tinyrad_free(*dictp);
      return(1);
   };

   return(0);
}


/* end of source */
