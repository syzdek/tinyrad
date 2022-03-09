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


# custom directories
rfcdocdir				= $(docdir)/rfc


# automake targets
check_PROGRAMS				=
doc_DATA				= AUTHORS.md \
					  ChangeLog.md \
					  COPYING.md \
					  DeveloperNotes.md \
					  README.md \
					  TODO.md
include_HEADERS				=
lib_LTLIBRARIES				=
lib_LIBRARIES				=
man_MANS				=
info_TEXINFOS				=
noinst_LTLIBRARIES			=
noinst_LIBRARIES			= lib/libtrutils.a
noinst_HEADERS				= include/tinyrad_compat.h \
					  include/tinyrad_internal.h \
					  include/trutils.h
noinst_PROGRAMS				=
bin_PROGRAMS				=
bin_SCRIPTS				=
pkgdata_DATA				=
rfcdoc_DATA				=
sbin_SCRIPTS				=
sbin_PROGRAMS				=
EXTRA_PROGRAMS				= examples/socket-example \
					  tests/test-array-sorted \
					  tests/test-array-stack \
					  tests/test-array-queue \
					  tests/test-assertions \
					  tests/test-assumptions \
					  tests/test-dict-attrs \
					  tests/test-dict-values \
					  tests/test-dict-vendors \
					  tests/test-oid-str \
					  tests/test-options \
					  tests/test-pckt-byte-order \
					  tests/test-url-desc2str \
					  tests/test-url-parse \
					  tests/test-url-resolve
EXTRA					= lib/libtinyrad.a \
					  lib/libtinyrad.la \
					  src/tinyrad \
					  src/tinyrad-dict \
					  src/tinyrad-url \
					  src/tinyradproxy \
					  include/tinyrad.h


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				=
TESTS					= tests/test-array-sorted \
					  tests/test-array-stack \
					  tests/test-array-queue \
					  tests/test-assertions \
					  tests/test-assumptions \
					  tests/test-dict-attrs \
					  tests/test-dict-defaults.sh \
					  tests/test-dict-values \
					  tests/test-dict-vendors \
					  tests/test-oid-str \
					  tests/test-options \
					  tests/test-pckt-byte-order \
					  tests/test-url-desc2str \
					  tests/test-url-parse \
					  tests/test-url-resolve
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= DeveloperNotes.md \
					  $(doc_DATA) \
					  doc/mans/tinyrad.3.in \
					  doc/mans/tinyrad_array.3.in \
					  doc/mans/tinyrad_error.3.in \
					  doc/mans/tinyrad_options.3.in \
					  doc/mans/tinyrad_queue.3.in \
					  doc/mans/tinyrad_stack.3.in \
					  doc/rfc/rfc-index.txt \
					  doc/rfc/rfc2865.txt \
					  doc/rfc/rfc2866.txt \
					  doc/rfc/rfc2867.txt \
					  doc/rfc/rfc2868.txt \
					  doc/rfc/rfc2869.txt \
					  doc/rfc/rfc3162.txt \
					  doc/rfc/rfc3575.txt \
					  doc/rfc/rfc6929.txt \
					  doc/rfc/rfc8044.txt \
					  include/tinyrad.h \
					  include/tinyrad_internal.h \
					  include/tinyrad_compat.h \
					  lib/libtinyrad/libtinyrad.sym \
					  tests/test-dict-defaults.sh
CLEANFILES				= $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  $(pkgdata_DATA) \
					  $(bin_SCRIPTS) \
					  $(sbin_SCRIPTS) \
					  $(EXTRA_PROGRAMS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.txz \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings \
					  --enable-utilities \
					  --enable-documentation \
					  --enable-examples


# conditional build
if ENABLE_DOCUMENATION
   rfcdoc_DATA				+= doc/rfc/rfc-index.txt
   rfcdoc_DATA				+= doc/rfc/rfc2865.txt
   rfcdoc_DATA				+= doc/rfc/rfc2866.txt
   rfcdoc_DATA				+= doc/rfc/rfc2867.txt
   rfcdoc_DATA				+= doc/rfc/rfc2868.txt
   rfcdoc_DATA				+= doc/rfc/rfc2869.txt
   rfcdoc_DATA				+= doc/rfc/rfc3162.txt
   rfcdoc_DATA				+= doc/rfc/rfc3575.txt
   rfcdoc_DATA				+= doc/rfc/rfc6929.txt
   rfcdoc_DATA				+= doc/rfc/rfc8044.txt
endif
if ENABLE_EXAMPLES
   noinst_PROGRAMS			+= examples/socket-example
endif
if ENABLE_LIBTINYRAD
   include_HEADERS			+= include/tinyrad.h
   lib_LTLIBRARIES			+= lib/libtinyrad.la
   man_MANS				+= doc/mans/tinyrad.3
   man_MANS				+= doc/mans/tinyrad_array.3
   man_MANS				+= doc/mans/tinyrad_error.3
   man_MANS				+= doc/mans/tinyrad_options.3
   man_MANS				+= doc/mans/tinyrad_queue.3
   man_MANS				+= doc/mans/tinyrad_stack.3
else
   noinst_HEADERS			+= include/tinyrad.h
   noinst_LIBRARIES			+= lib/libtinyrad.a
endif
if ENABLE_TINYRAD
   bin_PROGRAMS				+= src/tinyrad
endif
if ENABLE_TINYRAD_DICT
   bin_PROGRAMS				+= src/tinyrad-dict
   TESTS				+= tests/test-dict-defaults.sh
endif
if ENABLE_TINYRAD_URL
   bin_PROGRAMS				+= src/tinyrad-url
endif
if ENABLE_TINYRADPROXY
   sbin_PROGRAMS			+= src/tinyradproxy
endif


# macros for examples/socket-example
examples_socket_example_SOURCES		= examples/socket-example.c


# macros for lib/libtinyrad.a
lib_libtinyrad_a_DEPENDENCIES		= Makefile \
					  config.h
lib_libtinyrad_a_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libtinyrad
lib_libtinyrad_a_CFLAGS			= $(AM_CFLAGS)
lib_libtinyrad_a_SOURCES		= include/tinyrad.h \
					  include/tinyrad_internal.h \
					  include/tinyrad_compat.h \
					  lib/libtinyrad/libtinyrad.h \
					  lib/libtinyrad/larray.c \
					  lib/libtinyrad/larray.h \
					  lib/libtinyrad/ldata.c \
					  lib/libtinyrad/ldata.h \
					  lib/libtinyrad/ldebug.c \
					  lib/libtinyrad/ldebug.h \
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
					  lib/libtinyrad/loid.c \
					  lib/libtinyrad/loid.h \
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


# macros for lib/libtrutils.a
lib_libtrutils_a_DEPENDENCIES		= Makefile \
					  config.h
lib_libtrutils_a_CPPFLAGS		= $(AM_CPPFLAGS) \
					  -I$(srcdir)/lib/libtinyrad
lib_libtrutils_a_CFLAGS			= $(AM_CFLAGS)
lib_libtrutils_a_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  lib/libtrutils/libtrutils.h \
					  lib/libtrutils/lfile.c \
					  lib/libtrutils/lfile.h \
					  lib/libtrutils/llogging.c \
					  lib/libtrutils/llogging.h


# macros for src/tinyrad
src_tinyrad_DEPENDENCIES		= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_CFLAGS			= $(AM_CFLAGS)
src_tinyrad_CPPFLAGS			= $(AM_CPPFLAGS)
src_tinyrad_LDFLAGS			= $(AM_LDFLAGS)
src_tinyrad_LDADD			= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyrad_SOURCES			= include/tinyrad.h \
					  include/tinyrad_compat.h \
					  src/common.c \
					  src/common.h \
					  src/tinyrad.c


# macros for src/tinyrad-dict
src_tinyrad_dict_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
src_tinyrad_dict_LDADD			= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
src_tinyrad_dict_SOURCES		= include/tinyrad.h \
					  include/tinyrad_compat.h \
					  src/common.c \
					  src/common.h \
					  src/tinyrad-dict.c


# macros for src/tinyrad-url
src_tinyrad_url_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
src_tinyrad_url_LDADD			= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
src_tinyrad_url_SOURCES			= include/tinyrad.h \
					  include/tinyrad_compat.h \
					  src/common.c \
					  src/common.h \
					  src/tinyrad-url.c


# macros for src/tinyradproxy
src_tinyradproxy_DEPENDENCIES		= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyradproxy_CFLAGS			= $(AM_CFLAGS)
src_tinyradproxy_CPPFLAGS		= $(AM_CPPFLAGS)
src_tinyradproxy_LDFLAGS		= $(AM_LDFLAGS)
src_tinyradproxy_LDADD			= $(lib_LTLIBRARIES) \
					  $(noinst_LIBRARIES)
src_tinyradproxy_SOURCES		= include/tinyrad.h \
					  include/tinyrad_compat.h \
					  src/common.c \
					  src/common.h \
					  src/tinyradproxy.c


# macros for tests/tinyrad-array-sorted
tests_test_array_sorted_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_sorted_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_sorted_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-array-sorted.c


# macros for tests/tinyrad-array-stack
tests_test_array_stack_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_stack_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_stack_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-array-stack.c


# macros for tests/tinyrad-array-queue
tests_test_array_queue_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_queue_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_array_queue_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-array-queue.c


# macros for tests/tinyrad-assertions
tests_test_assertions_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_assertions_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_assertions_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-assertions.c


# macros for tests/tinyrad-dict-attrs
tests_test_dict_attrs_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_attrs_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_attrs_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/dict-data.c tests/dict-data.h \
					  tests/test-dict-attrs.c


# macros for tests/tinyrad-dict-values
tests_test_dict_values_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_values_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_values_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/dict-data.c tests/dict-data.h \
					  tests/test-dict-values.c


# macros for tests/tinyrad-dict-vendors
tests_test_dict_vendors_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_vendors_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_dict_vendors_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/dict-data.c tests/dict-data.h \
					  tests/test-dict-vendors.c


# macros for tests/test-oid-str
tests_test_oid_str_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_oid_str_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_oid_str_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-oid-str.c


# macros for tests/tinyrad-options
tests_test_options_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_options_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_options_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-options.c


# macros for tests/test-pckt-byte-order
tests_test_pckt_byte_order_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_pckt_byte_order_LDADD	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_pckt_byte_order_SOURCES	= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-pckt-byte-order.c


# macros for tests/test-url-desc2str
tests_test_url_desc2str_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_desc2str_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_desc2str_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-url-desc2str.c


# macros for tests/tinyrad-url-parse
tests_test_url_parse_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_parse_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_parse_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-url-parse.c


# macros for tests/tinyrad-url-resolve
tests_test_url_resolve_DEPENDENCIES	= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_resolve_LDADD		= $(lib_LTLIBRARIES) $(noinst_LIBRARIES)
tests_test_url_resolve_SOURCES		= $(noinst_HEADERS) $(include_HEADERS) \
					  tests/common.c tests/common.h \
					  tests/test-url-resolve.c


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-autotools.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-subst.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-version.am


# custom targets
.PHONY: examples

doc/mans/tinyrad.3: Makefile $(srcdir)/doc/mans/tinyrad.3.in
	@$(do_subst_dt)

doc/mans/tinyrad_array.3: Makefile $(srcdir)/doc/mans/tinyrad_array.3.in
	@$(do_subst_dt)

doc/mans/tinyrad_error.3: Makefile $(srcdir)/doc/mans/tinyrad_error.3.in
	@$(do_subst_dt)

doc/mans/tinyrad_options.3: Makefile $(srcdir)/doc/mans/tinyrad_options.3.in
	@$(do_subst_dt)

doc/mans/tinyrad_queue.3: Makefile $(srcdir)/doc/mans/tinyrad_queue.3.in
	@$(do_subst_dt)

doc/mans/tinyrad_stack.3: Makefile $(srcdir)/doc/mans/tinyrad_stack.3.in
	@$(do_subst_dt)

# local targets
install-exec-local:

install-data-local:

install-data-hook:
	if test -f $(DESTDIR)/$(mandir)/man3/tinyrad_array.3;then\
           cd $(DESTDIR)/$(mandir)/man3 && \
           ln -fs tinyrad_array.3 tinyrad_array_get.3 && \
           ln -fs tinyrad_array.3 tinyrad_array_add.3 && \
           ln -fs tinyrad_array.3 tinyrad_array_remove.3 && \
           ln -fs tinyrad_array.3 tinyrad_array_search.3; \
	fi;
	if test -f $(DESTDIR)/$(mandir)/man3/tinyrad_error.3;then\
           cd $(DESTDIR)/$(mandir)/man3 && \
           ln -fs tinyrad_error.3 tinyrad_strerror.3 && \
           ln -fs tinyrad_error.3 tinyrad_strerror_r.3; \
	fi;
	if test -f $(DESTDIR)/$(mandir)/man3/tinyrad_options.3;then\
           cd $(DESTDIR)/$(mandir)/man3 && \
           ln -fs tinyrad_options.3 tinyrad_get_option.3 && \
           ln -fs tinyrad_options.3 tinyrad_set_option.3; \
	fi;
	if test -f $(DESTDIR)/$(mandir)/man3/tinyrad_queue.3;then\
           cd $(DESTDIR)/$(mandir)/man3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_dequeue.3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_enqueue.3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_peek.3; \
	fi;
	if test -f $(DESTDIR)/$(mandir)/man3/tinyrad_stack.3;then\
           cd $(DESTDIR)/$(mandir)/man3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_peek.3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_pop.3 && \
           ln -fs tinyrad_stack.3 tinyrad_array_push.3; \
	fi;


uninstall-local:


uninstall-hook:
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_add.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_dequeue.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_enqueue.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_get.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_peek.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_pop.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_push.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_remove.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_array_search.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_get_option.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_set_option.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_strerror.3
	rm -f $(DESTDIR)/$(mandir)/man3/tinyrad_strerror_r.3


clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache

# end of automake file
