
Tiny RADIUS Client Library
==========================

This file contains miscellaneous notes from the developer regarding the
project design, system/compiler assumptions, and code style.  Please review
these notes before submitting patches to the developer.

Memory Management
=================

TinyRad (Tiny RADIUS Client Library) uses manual reference counters internally
to manage memory of nested data structures.  TinyRad data structures utilizing
reference counters must be declared with the first member of the type
`TinyRadObj` (`struct _tinyrad_obj`).  `TinyRadObj` contains a magic number, a
counter with an atomic data type, and a pointer to the function used to free the
memory and resources used by the data structure.  All data structures using
reference counters should be allocated using `tinyrad_obj_alloc()`.

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

   * `tinyrad_verify_is_obj()` returns `TRAD_YES` if the referenced memory uses
     reference counters and returns `TRAD_NO` if the referenced memory does not
     use reference counters.  `tinyrad_verify_is_obj` is primarily used by
     `tinyrad_free()` to distinguish between managed memory and unmanaged
     memory.

   * `tinyrad_free()` is a public function which uses `tinyrad_verify_is_obj()`
     to determine if a pointer references managed memory or unmanaged memory. If
     the memory is unmanaged, `tinyrad_free()` will call the equivalent of
     `fre()` to free the referenced memory. If the memory is managed, the
     `tinyrad_obj_release()` will be called.

Programs and libraries external to the TinyRad library must use `tinyrad_free()`
to free data structures and memory allocated by the TinyRad library.  A process
which calls either `tinyrad_free()` or `tinyrad_obj_release()` on managed memory
should consider the referenced memory unallocated.


Dictionary
==========

Object sort orders:

   * Vendors
     * Vendors By ID
       - Vendor ID
       - Order of Creation
     * Vendor By Name
       - Vendor Name

   * Attributes
     * Attributes by Name
       - Attribute Name
     * Attributes by Type
       - Vendor ID
       - Attribute OID
       - Attribute Order of Creation

   * Values
     * Values by Name
       - Attribute OID
       - Value Name
     * Values by Data
       - Attribute OID
       - Value Data
       - Order of Creation

