AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlbbcll1mon_client.la \
  libonlbbcll1mon_server.la

libonlbbcll1mon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonlbbcll1mon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


bbcll1includedir=$(pkgincludedir)/bbcll1

bbcll1include_HEADERS = \
  Bbcll1Mon.h \
  Bbcll1MonDraw.h

libonlbbcll1mon_server_la_SOURCES = \
  Bbcll1Mon.cc

libonlbbcll1mon_client_la_SOURCES = \
  Bbcll1MonDraw.cc

bin_SCRIPTS = \
  Bbcll1MonSetup.csh \
  Bbcll1MonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlbbcll1mon_server.la

testexternals_client_LDADD = \
  libonlbbcll1mon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
