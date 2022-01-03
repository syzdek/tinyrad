#
#   Tiny RADIUS Client Library
#   Copyright (C) 2021 David M. Syzdek <david@syzdek.net>.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of David M. Syzdek nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M SYZDEK BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
#
#   acinclude.m4 - custom m4 macros used by configure.ac
#


# AC_TINYRAD_IPV4
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_IPV4],[dnl
   WITH_IPV4=yes
   AM_CONDITIONAL([WITH_IPV4],    [test "${WITH_IPV4}" == "yes"])
   AM_CONDITIONAL([WITHOUT_IPV4], [test "${WITH_IPV4}" != "yes"])
])dnl


# AC_TINYRAD_IPV6
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_IPV6],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   enableval=""
   AC_ARG_WITH(
      ipv6,
      [AS_HELP_STRING([--without-ipv6], [disable IPv6 support])],
      [ WIPV6=$enableval ],
      [ WIPV6=$enableval ]
   )

   HAVE_IPV6=yes
   AC_MSG_CHECKING(for working IPv6 support)
   AC_COMPILE_IFELSE(
      [
         AC_LANG_PROGRAM(
            [[ #include <sys/types.h>
               #include <netinet/in.h>
               #include <sys/socket.h>
               #include <netdb.h>
            ]],
            [[ struct sockaddr_in6  a;
               struct addrinfo      hints;
               a.sin6_family  = AF_INET6;
               a.sin6_family  = PF_INET6;
               hints.ai_flags = AI_V4MAPPED|AI_ALL;
            ]]
         )
      ],
      [],
      [HAVE_IPV6="no"]
   )
   AC_MSG_RESULT($HAVE_IPV6)

   WITH_IPV6=yes
   if test "x${WIPV6}" = "xno";then
      WITH_IPV6=no
   elif test "x${WIPV6}" = "x";then
      WITH_IPV6=${HAVE_IPV6}
   else
      if test "x${HAVE_IPV6}" = "xno";then
         AC_MSG_ERROR([unable to determine IPv6 support])
      fi
   fi

   if test "x${WITH_IPV6}" == "xno";then
      AC_DEFINE_UNQUOTED(WITHOUT_IPV6, 1, [Disable IPv6])
   fi
   AM_CONDITIONAL([WITH_IPV6],    [test "${WITH_IPV6}" == "yes"])
   AM_CONDITIONAL([WITHOUT_IPV6], [test "${WITH_IPV6}" != "yes"])
])dnl


# AC_TINYRAD_LIBTINYRAD()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_LIBTINYRAD],[dnl
   enableval=""
   AC_ARG_ENABLE(
      libtinyrad,
      [AS_HELP_STRING([--disable-libtinyrad], [disable tinyrad library])],
      [ ELIBTINYRAD=$enableval ],
      [ ELIBTINYRAD=$enableval ]
   )
   if test "x${ELIBTINYRAD}" != "xno";then
      ENABLE_LIBTINYRAD="yes"
   else
      ENABLE_LIBTINYRAD="no"
   fi
   AM_CONDITIONAL([ENABLE_LIBTINYRAD],  [test "$ENABLE_LIBTINYRAD" = "yes"])
   AM_CONDITIONAL([DISABLE_LIBTINYRAD], [test "$ENABLE_LIBTINYRAD" = "no"])
])dnl


# AC_TINYRAD_TINYRAD()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_TINYRAD],[dnl

   AC_REQUIRE([AC_TINYRAD_UTILITIES])

   enableval=""
   AC_ARG_ENABLE(
      tinyrad,
      [AS_HELP_STRING([--disable-tinyrad], [disable tinyrad client])],
      [ ETINYRAD=$enableval ],
      [ ETINYRAD=$enableval ]
   )

   if test "x${ETINYRAD}" == "xyes";then
      ENABLE_TINYRAD="yes"
   elif test "x${ETINYRAD}" == "xno";then
      ENABLE_TINYRAD="no"
   elif test "x${ENABLE_UTILITIES}" == "xyes";then
      ENABLE_TINYRAD="yes"
   elif test "x${ENABLE_UTILITIES}" == "xno";then
      ENABLE_TINYRAD="no"
   else
      ENABLE_TINYRAD="yes"
   fi

   AM_CONDITIONAL([ENABLE_TINYRAD],  [test "$ENABLE_TINYRAD" = "yes"])
   AM_CONDITIONAL([DISABLE_TINYRAD], [test "$ENABLE_TINYRAD" = "no"])
])dnl


# AC_TINYRAD_TINYRAD_DICT()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_TINYRAD_DICT],[dnl

   AC_REQUIRE([AC_TINYRAD_UTILITIES])

   enableval=""
   AC_ARG_ENABLE(
      tinyrad-dict,
      [AS_HELP_STRING([--enable-tinyrad-dict], [enable tinyrad-dict utility])],
      [ ETINYRADDICT=$enableval ],
      [ ETINYRADDICT=$enableval ]
   )

   if test "x${ETINYRADDICT}" == "xyes";then
      ENABLE_TINYRAD_DICT="yes"
   elif test "x${ETINYRADDICT}" == "xno";then
      ENABLE_TINYRAD_DICT="no"
   elif test "x${ENABLE_UTILITIES}" == "xyes";then
      ENABLE_TINYRAD_DICT="yes"
   elif test "x${ENABLE_UTILITIES}" == "xno";then
      ENABLE_TINYRAD_DICT="no"
   else
      ENABLE_TINYRAD_DICT="no"
   fi

   AM_CONDITIONAL([ENABLE_TINYRAD_DICT],  [test "$ENABLE_TINYRAD_DICT" = "yes"])
   AM_CONDITIONAL([DISABLE_TINYRAD_DICT], [test "$ENABLE_TINYRAD_DICT" = "no"])
])dnl


# AC_TINYRAD_TINYRAD_URL()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_TINYRAD_URL],[dnl

   AC_REQUIRE([AC_TINYRAD_UTILITIES])

   enableval=""
   AC_ARG_ENABLE(
      tinyrad-url,
      [AS_HELP_STRING([--enable-tinyrad-url], [enable tinyrad-url utility])],
      [ ETINYRADURL=$enableval ],
      [ ETINYRADURL=$enableval ]
   )

   if test "x${ETINYRADURL}" == "xyes";then
      ENABLE_TINYRAD_URL="yes"
   elif test "x${ETINYRADURL}" == "xno";then
      ENABLE_TINYRAD_URL="no"
   elif test "x${ENABLE_UTILITIES}" == "xyes";then
      ENABLE_TINYRAD_URL="yes"
   elif test "x${ENABLE_UTILITIES}" == "xno";then
      ENABLE_TINYRAD_URL="no"
   else
      ENABLE_TINYRAD_URL="no"
   fi

   AM_CONDITIONAL([ENABLE_TINYRAD_URL],  [test "$ENABLE_TINYRAD_URL" = "yes"])
   AM_CONDITIONAL([DISABLE_TINYRAD_URL], [test "$ENABLE_TINYRAD_URL" = "no"])
])dnl


# AC_TINYRAD_TINYRADPROXY()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_TINYRADPROXY],[dnl

   AC_REQUIRE([AC_TINYRAD_UTILITIES])

   enableval=""
   AC_ARG_ENABLE(
      tinyradproxy,
      [AS_HELP_STRING([--enable-tinyradproxy], [enable tinyrad proxy])],
      [ ETINYRADPROXY=$enableval ],
      [ ETINYRADPROXY=$enableval ]
   )

   if test "x${ETINYRADPROXY}" == "xyes";then
      ENABLE_TINYRADPROXY="yes"
   elif test "x${ETINYRADPROXY}" == "xno";then
      ENABLE_TINYRADPROXY="no"
   elif test "x${ENABLE_UTILITIES}" == "xyes";then
      ENABLE_TINYRADPROXY="yes"
   elif test "x${ENABLE_UTILITIES}" == "xno";then
      ENABLE_TINYRADPROXY="no"
   else
      ENABLE_TINYRADPROXY="no"
   fi

   AM_CONDITIONAL([ENABLE_TINYRADPROXY],  [test "$ENABLE_TINYRADPROXY" = "yes"])
   AM_CONDITIONAL([DISABLE_TINYRADPROXY], [test "$ENABLE_TINYRADPROXY" = "no"])
])dnl


# AC_TINYRAD_UTILS()
# ______________________________________________________________________________
AC_DEFUN([AC_TINYRAD_UTILITIES],[dnl
   enableval=""
   AC_ARG_ENABLE(
      utilities,
      [AS_HELP_STRING([--enable-utilities], [enable all RADIUS utility])],
      [ EUTILITIES=$enableval ],
      [ EUTILITIES=$enableval ]
   )
   if test "x${EUTILITIES}" == "xyes";then
      ENABLE_UTILITIES="yes"
   elif test "x${EUTILITIES}" == "xno";then
      ENABLE_UTILITIES="no"
   else
      ENABLE_UTILITIES="auto"
   fi
])dnl

# end of m4 file
