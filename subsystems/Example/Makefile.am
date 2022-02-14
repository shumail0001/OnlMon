AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.C

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLMON_MAIN)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include 

lib_LTLIBRARIES = \
  libonlmymon.la

libonlmymon_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLMON_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonclient \
  -lonlmondb

noinst_HEADERS = \
  MyMon.h

include_HEADERS =

libonlmymon_la_SOURCES = \
  MyMon_Dict.C \
  MyMonDraw.C \
  MyMon.C

bin_SCRIPTS = \
  MyMonsetup.csh

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmymon.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


MyMon_Dict.C: MyMon.h MyMonDraw.h MyMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
