
Tiny RADIUS Client Library
==========================

Copyright (C) 2022 David M. Syzdek <david@syzdek.net>.  
All rights reserved.  

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of David M. Syzdek nor the
     names of its contributors may be used to endorse or promote products
     derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.


Table of Contents
-----------------

   * Overview
   * Coding Style and Guidelines
   * Memory Management
   * Tiny RADIUS Dictionary
     - Dictionary Vendor
     - Dictionary Attribute
     - Dictionary Value
     - Dictionary OID
     - Dictionary Thread Safety


Overview
========

This file contains miscellaneous notes from the developer regarding the
project design, system/compiler assumptions, and code style of TinyRad (Tiny
RADIUS Client Library).  Please review these notes before submitting patches
to the developer.


Coding Style and Guidelines
===========================

To be written.


Memory Management
=================

TinyRad uses manual reference counters internally to manage memory of nested
data structures.  TinyRad data structures utilizing reference counters must be
declared with the first member of the type `TinyRadObj`
(`struct _tinyrad_obj`).  `TinyRadObj` contains a magic number, a counter with
an atomic data type, and a pointer to the function used to free the memory and
resources used by the data structure.  All data structures using reference
counters should be allocated using `tinyrad_obj_alloc()`.

The TinyRad library uses the following functions to manage data structures
utilizing reference counters:

   * `tinyrad_obj_alloc()` allocates memory for a managed data structure. The
     reference counter is initialized to zero.  Functions calling
     `tinyrad_obj_alloc()` must call `tinyrad_obj_retain()` or
     `tinyrad_obj_release()` before exiting.

   * `tinyrad_obj_retain()` increments the reference counter using an atomic
     operation and returns a pointer to the data structure retained.

   * `tinyrad_obj_retain_count()` returns the value of the current reference
     counter of the referenced data structure.

   * `tinyrad_obj_release()` decrements the reference counter using an atomic
     operation. If the reference counter equals zero or less, the  function
     referenced in the TinyRadObj member will be called to free the memory and
     resources.

   * `tinyrad_verify_is_obj()` returns `TRAD_YES` if the referenced memory
     uses reference counters and returns `TRAD_NO` if the referenced memory
     does not use reference counters.  `tinyrad_verify_is_obj` is primarily
     used by `tinyrad_free()` to distinguish between managed memory and
     unmanaged memory.

   * `tinyrad_free()` is a public function which uses the function
     `tinyrad_verify_is_obj()` to determine if a pointer references managed
     memory or unmanaged memory.  If the memory is unmanaged, `tinyrad_free()`
     will call the equivalent of `free()` to free the referenced memory. If
     the memory is managed, then the function `tinyrad_obj_release()` will be
     called.

Unless otherwise specified in the TinyRad documentation, programs and
libraries external to the TinyRad library must use the `tinyrad_free()`
function to free data structures and memory allocated by the TinyRad library.
A process which calls either `tinyrad_free()` or `tinyrad_obj_release()` on
managed memory should consider the referenced memory unallocated.


Tiny RADIUS Dictionary
======================

The TinyRad dictionary containes definitions for vendors (`TinyRadDictVendor`),
attributes (`TinyRadDictAttr`), and values (`TinyRadDictValue`).  These three
entity types are used to convert between user friendly names and binary data.

  The first entity,
is the `TinyRadDictAttr`. `TinyRadDictAttr` is the primary entity and
represents a RADIUS attribute definition.  A `TinyRadDictAttr` may reference a
`TinyRadDictVendor` which contains a vendor definition of one or more
attributes. A `TinyRadDictVendor` will contain the vendor's common
specifications for its attributes such as "length octets" and "type octets".
Some attributes have enumerated values, these are defined in
`TinyRadDictValue`.  Each `TinyRadDictValue` references the associated
`TinyRadDictAttr`.


Dictionary Vendor
-----------------

`TinyRadDictVendor` represents a RADIUS vendor. A `TinyRadDictVendor` will
define the number of octets used to specify the vendor's attribute length
(`len_octets`) and define the number of octets used to specify the vendor's
attribute type (`type_octets`). If an unknown vendor ID is encountered, the
library will assume the vendor uses a 1 byte "vendor length" and a 1 byte
"vendor type" as illustrated by the example in section 5.26 of RFC 2865. The
dictionary reference maintains the definitions of `TinyRadDictVendor` in an
array sorted by name and in an array nominally sorted by Vendor-Id.

The array sorted by name must not contain duplicate names and sorts using the
following criteria:

   * Vendor Name

The array sorted nominally by Vendor-Id may contain duplicate Vendor-ID
values if the Vendor-Id, `type_octets` and `len_octets` are identical among
the vendors with duplicate Vendor-Id.  The array sorted nominally by Vendor-Id
uses the following sort criteria:

   * Vendor-Id
   * Order of Creation


Dictionary Attribute
--------------------

`TinyRadDictAttr` represents a RADIUS attribute definition.  If the attribute
is specific to a vendor, then the `TinyRadDictAttr` will reference a
`TinyRadDictVendor`.  The dictionary reference maintains the definitions of
`TinyRadDictAttr` in an array sorted by name and in an array nominally sorted
by OID.

The array sorted by name must not contain duplicate names and sorts using the
following criteria:

   * Attribute Name

The array sorted nominally by OID may contain duplicate OID values if the OID,
vendor, data type, and flags are identical among the attributes with duplicate
OID.  Allowing duplicate OID allow an attribute to be referenced by multiple
names (i.e. if a later RFC updates the name of an attribute defined in a
previous RFC). The array sorted nominally by OID uses the following
sort criteria:

   * Vendor-Id
   * Attribute OID
   * Order of Creation


Dictionary Value
----------------

`TinyRadDictValue` represents an enumerated value of a RADIUS attribute of
data types enum, integer, integer64, byte, short, and signed.  The dictionary
reference maintains the definitions of `TinyRadDictValue` in an array
nominally sorted by name and in an array nominally sorted by OID.

The array sorted nominally by name must not contain duplicate value names for
same attribute. Value names may be duplicated in different attributes. The
array nominally sorted by name uses the following criteria:

   * OID of Associated Attribute
   * Value Name

The array sorted nominally by name may contain duplicate values for the same
attribute. The array sorted nominally by OID uses the following sort criteria:

   * OID of Associated Attribute
   * Value (i.e. integer data)
   * Order of Creation


Dictionary OID
--------------

Internally, individual `TinyRadDictAttr` are uniquely identified using a
numeric OID (`TinyRadOID`).  `TinyRadDictVendor` are indirectly identified
in `TinyRadDictAttr` and `TinyRadDictValue` using a numeric OID. The first
element of the OID always identifies a RADIUS attribute type defined by an
RFC. The value of each subsequent element of an OID is dependent upon the
attribute type and contains either an optional attribute sub-type or an
optional Vendor-ID.

The simplest OID consists of just an attribute type such as :

    2

Where:

   * `2` is the type of the *User-Password* attribute (RFC 2865 Section 5.2)

An example of an OID for a Vendor-Specific attribute may be:

    26.32473.57

Where:

   * `26` is the type of the *Vendor-Specific* attribute (RFC 2865 Section 5.26)
   * `32473` is the Vendor-Id (example Enterprise Number defined in RFC 5612)
   * `57` is the vendor type

An example of an OID for an attribute of the EVS (Extended-Vendor-Specific)
data type which contains two TLV (Type-Length-Value) data subtypes may be:

    243.26.32473.122.54.231

Where:

   * `243` is the type of the *Extended-Type-3* attribute (RFC 6929 Section 3.3)
   * `26` is the Extended-Type for Vendor sub-types (RFC 6929 Section 4.3)
   * `32473` is the Vendor-Id (example Enterprise Number defined in RFC 5612)
   * `122` is the vendor sub-type
   * `54` is the TLV sub-type of `243.26.32473.122`
   * `231` is the TLB sub-type of `243.26.32473.122.54`


Dictionary Thread Safety
------------------------

The dictionary reference is not thread safe for read-write operations due to
the lack of internal locks, however a dictionary reference can be safely used
for read-only operations by multiple threads simultaneously.  Multiple TinyRad
functions retain references and share references to the dictionary, so
external locks are unable to ensure thread-saftey across mutiple threads for
read-write access to the dictionary.

A dictionary reference can be marked as read-only to ensure that it is not
modified in a multi-threaded application after the definitions have been
imported or loaded from defaults.  The following example snippet demonstrates
how to make the dictionary read-only:

    int opt = TRAD_YES;
    tinyrad_dict_set_option(dict, TRAD_DICT_OPT_READONLY, &opt);

Setting the dictionary to read-only disables the following functions for the
specific dictionary reference:

   * `tinyrad_dict_defaults()`
   * `tinyrad_dict_import()`
   * `tinyrad_dict_parse()`
   * `tinyrad_dict_set_option()`

