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
#define _TESTS_COMMON_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
#pragma mark - Headers

#include <stdio.h>
#include <stdarg.h>

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

const char * test_urldesc_strs_bad[] =
{
   "rad://www.foo.org",

   "radius:///drowssap",
   "radius:/www.foo.org/drowssap",
   "radius:///www.foo.org/drowssap",
   "radius://www.foo.org",
   "radius://www.foo.org/",
   "radius://www.foo.org/?",
   "radius://www.foo.org/?udp",
   "radius://www.foo.org/?tcp",
   "radius://www.foo.org/drowssap?",
   "radius://www.foo.org/drowssap?u",
   "radius://www.foo.org:",
   "radius://www.foo.org:1111",
   "radius://www.foo.org:1111/",
   "radius://www.foo.org:1111/drowssap?",
   "radius://www.foo.org:1111/drowssap?u",

   "radius-acct:///drowssap",
   "radius-acct:/www.foo.org/drowssap",
   "radius-acct:///www.foo.org/drowssap",
   "radius-acct://www.foo.org",
   "radius-acct://www.foo.org/",
   "radius-acct://www.foo.org/?",
   "radius-acct://www.foo.org/?udp",
   "radius-acct://www.foo.org/?tcp",
   "radius-acct://www.foo.org/drowssap?",
   "radius-acct://www.foo.org/drowssap?u",
   "radius-acct://www.foo.org:",
   "radius-acct://www.foo.org:1111",
   "radius-acct://www.foo.org:1111/",
   "radius-acct://www.foo.org:1111/drowssap?",
   "radius-acct://www.foo.org:1111/drowssap?u",

   "radius-dynauth:///drowssap",
   "radius-dynauth:/www.foo.org/drowssap",
   "radius-dynauth:///www.foo.org/drowssap",
   "radius-dynauth://www.foo.org",
   "radius-dynauth://www.foo.org/",
   "radius-dynauth://www.foo.org/?",
   "radius-dynauth://www.foo.org/?udp",
   "radius-dynauth://www.foo.org/?tcp",
   "radius-dynauth://www.foo.org/drowssap?",
   "radius-dynauth://www.foo.org/drowssap?u",
   "radius-dynauth://www.foo.org:",
   "radius-dynauth://www.foo.org:1111",
   "radius-dynauth://www.foo.org:1111/",
   "radius-dynauth://www.foo.org:1111/drowssap?",
   "radius-dynauth://www.foo.org:1111/drowssap?u",

   "radsec:///drowssap",
   "radsec:/www.foo.org/",
   "radsec:/www.foo.org/drowssap",
   "radsec:///www.foo.org/",
   "radsec:///www.foo.org/drowssap",
   "radsec://www.foo.org/drowssap",
   "radsec://www.foo.org/?",
   "radsec://www.foo.org/drowssap?",
   "radsec://www.foo.org/drowssap?u",
   "radsec://www.foo.org:",
   "radsec://www.foo.org:1111/drowssap?",
   "radsec://www.foo.org:1111/drowssap?u",

   NULL
};


const char * test_urldesc_strs_desc2str[] =
{
   "radius://www.foo.org/drowssap",
   "radius://www.foo.org/drowssap?tcp",
   "radius://www.foo.org:1111/drowssap",
   "radius://www.foo.org:1111/drowssap?tcp",
   "radius://[::1]/drowssap",
   "radius://[::1]/drowssap?tcp",
   "radius://[::1]:1111/drowssap",
   "radius://[::1]:1111/drowssap?tcp",

   "radius-acct://www.foo.org/drowssap",
   "radius-acct://www.foo.org/drowssap?tcp",
   "radius-acct://www.foo.org:1111/drowssap",
   "radius-acct://www.foo.org:1111/drowssap?tcp",
   "radius-acct://[::1]/drowssap",
   "radius-acct://[::1]/drowssap?tcp",
   "radius-acct://[::1]:1111/drowssap",
   "radius-acct://[::1]:1111/drowssap?tcp",

   "radius-dynauth://www.foo.org/drowssap",
   "radius-dynauth://www.foo.org/drowssap?tcp",
   "radius-dynauth://www.foo.org:1111/drowssap",
   "radius-dynauth://www.foo.org:1111/drowssap?tcp",
   "radius-dynauth://[::1]/drowssap",
   "radius-dynauth://[::1]/drowssap?tcp",
   "radius-dynauth://[::1]:1111/drowssap",
   "radius-dynauth://[::1]:1111/drowssap?tcp",

   "radsec://www.foo.org/",
   "radsec://www.foo.org/?udp",
   "radsec://www.foo.org:1111/",
   "radsec://www.foo.org:1111/?udp",
   "radsec://[::1]/",
   "radsec://[::1]/?udp",
   "radsec://[::1]:1111/",
   "radsec://[::1]:1111/?udp",
   NULL
};


const char * test_urldesc_strs_good[] =
{
   "radius://www.foo.org/drowssap",
   "radius://www.foo.org/drowssap?tcp",
   "radius://www.foo.org/drowssap?udp",
   "radius://www.foo.org:1111/drowssap",
   "radius://www.foo.org:1111/drowssap?tcp",
   "radius://[::1]/drowssap",
   "radius://[::1]/drowssap?tcp",
   "radius://[::1]/drowssap?udp",
   "radius://[::1]:1111/drowssap",
   "radius://[::1]:1111/drowssap?tcp",
   "radius://[::1]:1111/drowssap?udp",

   "radius-acct://www.foo.org/drowssap",
   "radius-acct://www.foo.org/drowssap?tcp",
   "radius-acct://www.foo.org/drowssap?udp",
   "radius-acct://www.foo.org:1111/drowssap",
   "radius-acct://www.foo.org:1111/drowssap?tcp",
   "radius-acct://www.foo.org:1111/drowssap?udp",
   "radius-acct://[::1]/drowssap",
   "radius-acct://[::1]/drowssap?tcp",
   "radius-acct://[::1]/drowssap?udp",
   "radius-acct://[::1]:1111/drowssap",
   "radius-acct://[::1]:1111/drowssap?tcp",
   "radius-acct://[::1]:1111/drowssap?udp",

   "radius-dynauth://www.foo.org/drowssap",
   "radius-dynauth://www.foo.org/drowssap?tcp",
   "radius-dynauth://www.foo.org/drowssap?udp",
   "radius-dynauth://www.foo.org:1111/drowssap",
   "radius-dynauth://www.foo.org:1111/drowssap?tcp",
   "radius-dynauth://www.foo.org:1111/drowssap?udp",
   "radius-dynauth://[::1]/drowssap",
   "radius-dynauth://[::1]/drowssap?tcp",
   "radius-dynauth://[::1]/drowssap?udp",
   "radius-dynauth://[::1]:1111/drowssap",
   "radius-dynauth://[::1]:1111/drowssap?tcp",
   "radius-dynauth://[::1]:1111/drowssap?udp",

   "radsec://www.foo.org",
   "radsec://www.foo.org/",
   "radsec://www.foo.org/?tcp",
   "radsec://www.foo.org/?udp",
   "radsec://www.foo.org:1111",
   "radsec://www.foo.org:1111/",
   "radsec://www.foo.org:1111/?tcp",
   "radsec://www.foo.org:1111/?udp",
   "radsec://[::1]",
   "radsec://[::1]/",
   "radsec://[::1]/?tcp",
   "radsec://[::1]/?udp",
   "radsec://[::1]:1111",
   "radsec://[::1]:1111/",
   "radsec://[::1]:1111/?tcp",
   "radsec://[::1]:1111/?udp",

   NULL
};


const char * test_urldesc_strs_resolvable[] =
{
   "radius://203.0.113.45/drowssap",
   "radius://203.0.113.45/drowssap?tcp",
   "radius://203.0.113.45/drowssap?udp",
   "radius://203.0.113.45:1111/drowssap",
   "radius://203.0.113.45:1111/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",
   "radius://localhost/drowssap",
   "radius://localhost/drowssap?tcp",
   "radius://localhost/drowssap?udp",
   "radius://localhost:1111/drowssap",
   "radius://localhost:1111/drowssap?tcp",

   "radius-acct://203.0.113.45/drowssap",
   "radius-acct://203.0.113.45/drowssap?tcp",
   "radius-acct://203.0.113.45/drowssap?udp",
   "radius-acct://203.0.113.45:1111/drowssap",
   "radius-acct://203.0.113.45:1111/drowssap?tcp",
   "radius-acct://203.0.113.45:1111/drowssap?udp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius-acct://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",
   "radius-acct://localhost/drowssap",
   "radius-acct://localhost/drowssap?tcp",
   "radius-acct://localhost/drowssap?udp",
   "radius-acct://localhost:1111/drowssap",
   "radius-acct://localhost:1111/drowssap?tcp",
   "radius-acct://localhost:1111/drowssap?udp",

   "radius-dynauth://203.0.113.45/drowssap",
   "radius-dynauth://203.0.113.45/drowssap?tcp",
   "radius-dynauth://203.0.113.45/drowssap?udp",
   "radius-dynauth://203.0.113.45:1111/drowssap",
   "radius-dynauth://203.0.113.45:1111/drowssap?tcp",
   "radius-dynauth://203.0.113.45:1111/drowssap?udp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?tcp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/drowssap?udp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?tcp",
   "radius-dynauth://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/drowssap?udp",
   "radius-dynauth://localhost/drowssap",
   "radius-dynauth://localhost/drowssap?tcp",
   "radius-dynauth://localhost/drowssap?udp",
   "radius-dynauth://localhost:1111/drowssap",
   "radius-dynauth://localhost:1111/drowssap?tcp",
   "radius-dynauth://localhost:1111/drowssap?udp",

   "radsec://203.0.113.45",
   "radsec://203.0.113.45/",
   "radsec://203.0.113.45/?tcp",
   "radsec://203.0.113.45/?udp",
   "radsec://203.0.113.45:1111",
   "radsec://203.0.113.45:1111/",
   "radsec://203.0.113.45:1111/?tcp",
   "radsec://203.0.113.45:1111/?udp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/?tcp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]/?udp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/?tcp",
   "radsec://[2001:dB8:ffed:1a:9c4a:14fe:113a:ce4b]:1111/?udp",
   "radsec://localhost",
   "radsec://localhost/",
   "radsec://localhost/?tcp",
   "radsec://localhost/?udp",
   "radsec://localhost:1111",
   "radsec://localhost:1111/",
   "radsec://localhost:1111/?tcp",
   "radsec://localhost:1111/?udp",

   NULL
};




//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#pragma mark - Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#pragma mark - Functions


/* end of source */
