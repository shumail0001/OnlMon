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
  libonlmbdll1mon_client.la \
  libonlmbdll1mon_server.la

libonlmbdll1mon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb

libonlmbdll1mon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


mbdll1includedir=$(pkgincludedir)/mbdll1

mbdll1include_HEADERS = \
  Mbdll1Mon.h \
  Mbdll1MonDraw.h

libonlmbdll1mon_server_la_SOURCES = \
  Mbdll1Mon.cc

libonlmbdll1mon_client_la_SOURCES = \
  Mbdll1MonDraw.cc

bin_SCRIPTS = \
  Mbdll1Monsetup.csh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlmbdll1mon_server.la

testexternals_client_LDADD = \
  libonlmbdll1mon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
