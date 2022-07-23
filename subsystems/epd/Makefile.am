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
  libonlepdmon_client.la \
  libonlepdmon_server.la

libonlepdmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonlepdmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


epdincludedir=$(pkgincludedir)/epd

epdinclude_HEADERS = \
  EpdMon.h \
  EpdMonDraw.h

libonlepdmon_server_la_SOURCES = \
  EpdMon.cc

libonlepdmon_client_la_SOURCES = \
  EpdMonDraw.cc

bin_SCRIPTS = \
  EpdMonSetup.csh \
  EpdMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlepdmon_server.la

testexternals_client_LDADD = \
  libonlepdmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
