AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlinttmon_client.la \
  libonlinttmon_server.la

libonlinttmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonlinttmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


inttincludedir=$(pkgincludedir)/intt

inttinclude_HEADERS = \
  InttMon.h \
  InttMonDraw.h \
  InttExecs.h \
  InttConstants.h

libonlinttmon_server_la_SOURCES = \
  InttMon.cc

libonlinttmon_client_la_SOURCES = \
  InttMonDraw.cc

bin_SCRIPTS = \
  InttMonSetup.csh \
  InttMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client

BUILT_SOURCES = testexternals.cc

testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlinttmon_server.la

testexternals_client_LDADD = \
  libonlinttmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
