
Tiny RADIUS Client Library
==========================

This file contains miscellaneous notes from the developer regarding the
project design, system/compiler assumptions, and code style.  Please review
these notes before submitting patches to the developer.


Dictionary
----------

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

