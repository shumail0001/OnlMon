AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonllocalpolmon_client.la \
  libonllocalpolmon_server.la

libonllocalpolmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonutils \
  -lonlmondb

libonllocalpolmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


localpolincludedir=$(pkgincludedir)/localpol

localpolinclude_HEADERS = \
  LocalPolMon.h \
  LocalPolMonDraw.h

libonllocalpolmon_server_la_SOURCES = \
  LocalPolMon.cc

libonllocalpolmon_client_la_SOURCES = \
  LocalPolMonDraw.cc

bin_SCRIPTS = \
  LocalPolMonSetup.csh \
  LocalPolMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonllocalpolmon_server.la

testexternals_client_LDADD = \
  libonllocalpolmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
