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
  libonlmymon.la

libonlmymon_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb

noinst_HEADERS = \
  MyMon.h

include_HEADERS =

libonlmymon_la_SOURCES = \
  MyMonDraw.cc \
  MyMon.cc

bin_SCRIPTS = \
  MyMonsetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libonlmymon.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


MyMon_Dict.cc: MyMon.h MyMonDraw.h MyMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
