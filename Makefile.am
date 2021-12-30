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
#   @configure_input@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					=
DIST_SUBDIRS				=

# GNU make options
V					= 0


# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 \
					  -DHAVE_CONFIG_H=1 \
					  -I$(top_builddir)/include \
					  -I$(top_srcdir)/include \
					  -DSYSCONFDIR="\"$(sysconfdir)\""
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
AM_MAKEINFOFLAGS                        = --no-split
DEFS					=


# automake targets
check_PROGRAMS				=
doc_DATA				= \
					  AUTHORS.md \
					  ChangeLog.md \
					  COPYING.md \
					  README.md \
					  TODO.md
include_HEADERS				=
lib_LTLIBRARIES				=
lib_LIBRARIES				=
man_MANS				=
info_TEXINFOS				=
noinst_LTLIBRARIES			=
noinst_LIBRARIES			=
noinst_HEADERS				=
noinst_PROGRAMS				= tests/tinyrad-url-desc2str \
					  tests/tinyrad-url-parse \
					  tests/tinyrad-url-resolve
bin_PROGRAMS				=
bin_SCRIPTS				=
pkgdata_DATA				=
sbin_SCRIPTS				=
sbin_PROGRAMS				=
EXTRA					= lib/libtinyrad.a \
					  lib/libtinyrad.la \
					  src/tinyrad \
					  src/tinyrad-url \
					  src/tinyradproxy \
					  include/tinyrad.h


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				=
TESTS					= \
					  tests/tinyrad-url-desc2str \
					  tests/tinyrad-url-parse \
					  tests/tinyrad-url-resolve
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= \
					  $(doc_DATA) \
					  include/tinyrad.h \
					  lib/libtinyrad/libtinyrad.sym
CLEANFILES				= \
					  $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  $(pkgdata_DATA) \
					  $(bin_SCRIPTS) \
					  $(sbin_SCRIPTS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.txz \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings \
					  --enable-tinyradproxy


# conditional build
if ENABLE_LIBTINYRAD
   include_HEADERS			+= include/tinyrad.h
   lib_LTLIBRARIES			+= lib/libtinyrad.la
else
   noinst_HEADERS			+= include/tinyrad.h
   noinst_LIBRARIES			+= lib/libtinyrad.a
endif
if ENABLE_TINYRAD
   bin_PROGRAMS				+= src/tinyrad
endif
if WANT_TINYRAD_URL
   bin_PROGRAMS				+= src/tinyrad-url
endif
if WANT_TINYRADPROXY
   sbin_PROGRAMS			+= src/tinyradproxy
endif


# macros for lib/libtinyrad.a
lib_libtinyrad_a_DEPENDENCIES		= Makefile \
					  config.h
lib_libtinyrad_a_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libtinyrad
lib_libtinyrad_a_CFLAGS			= $(AM_CFLAGS)
lib_libtinyrad_a_SOURCES		= include/tinyrad.h \
					  lib/libtinyrad/libtinyrad.h \
					  lib/libtinyrad/ldict.c \
					  lib/libtinyrad/ldict.h \
					  lib/libtinyrad/lerror.c \
					  lib/libtinyrad/lerror.h \
					  lib/libtinyrad/lfile.c \
					  lib/libtinyrad/lfile.h \
					  lib/libtinyrad/lmap.c \
					  lib/libtinyrad/lmap.h \
					  lib/libtinyrad/lmemory.c \
					  lib/libtinyrad/lmemory.h \
					  lib/libtinyrad/lnet.c \
					  lib/libtinyrad/lnet.h \
					  lib/libtinyrad/lproto.c \
					  lib/libtinyrad/lproto.h \
					  lib/libtinyrad/lurl.c \
					  lib/libtinyrad/lurl.h


# macros for lib/libtinyrad.la
lib_libtinyrad_la_DEPENDENCIES		= Makefile \
					  config.h \
					  lib/libtinyrad/libtinyrad.sym
lib_libtinyrad_la_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libtinyrad
lib_libtinyrad_la_CFLAGS		= $(AM_CFLAGS)
lib_libtinyrad_la_LDFLAGS		= -rpath'$(libdir)' \
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
					  -export-symbols $(srcdir)/lib/libtinyrad/libtinyrad.sym
lib_libtinyrad_la_SOURCES		= $(lib_libtinyrad_a_SOURCES)


# macros for src/tinyrad
src_tinyrad_DEPENDENCIES		= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_CFLAGS			= $(AM_CFLAGS)
src_tinyrad_CPPFLAGS			= $(AM_CPPFLAGS)
src_tinyrad_LDFLAGS			= $(AM_LDFLAGS)
src_tinyrad_LDADD			= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_SOURCES			= include/tinyrad.h \
					  src/utilities/common.c \
					  src/utilities/common.h \
					  src/utilities/tinyrad.c


# macros for src/tinyrad
src_tinyrad_DEPENDENCIES		= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_url_CFLAGS			= $(AM_CFLAGS)
src_tinyrad_url_CPPFLAGS		= $(AM_CPPFLAGS)
src_tinyrad_url_LDFLAGS			= $(AM_LDFLAGS)
src_tinyrad_url_LDADD			= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_url_SOURCES			= include/tinyrad.h \
					  src/utilities/common.c \
					  src/utilities/common.h \
					  src/utilities/tinyrad-url.c


# macros for src/tinyradproxy
src_tinyradproxy_DEPENDENCIES		= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyradproxy_CFLAGS			= $(AM_CFLAGS)
src_tinyradproxy_CPPFLAGS		= $(AM_CPPFLAGS)
src_tinyradproxy_LDFLAGS		= $(AM_LDFLAGS)
src_tinyradproxy_LDADD			= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyradproxy_SOURCES		= include/tinyrad.h \
					  src/proxy/common.h \
					  src/proxy/tinyradproxy.c


# macros for tests/tinyrad-url-desc2str
tests_tinyrad_url_desc2str_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_tinyrad_url_desc2str_LDADD	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)


# macros for tests/tinyrad-url-parse
tests_tinyrad_url_parse_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_tinyrad_url_parse_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)


# macros for tests/tinyrad-url-resolve
tests_tinyrad_url_resolve_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_tinyrad_url_resolve_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-autotools.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-subst.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-version.am


# custom targets
.PHONY:

# local targets
install-exec-local:

install-data-local:

install-data-hook:

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache

# end of automake file
