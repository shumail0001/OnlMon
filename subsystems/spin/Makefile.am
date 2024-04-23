AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlspinmon_client.la \
  libonlspinmon_server.la

libonlspinmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmonutils \
  -lXingShiftCal \
  -lonlmondb

libonlspinmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


spinincludedir=$(pkgincludedir)/spin

spininclude_HEADERS = \
  SpinMon.h \
  SpinMonDraw.h

libonlspinmon_server_la_SOURCES = \
  SpinMon.cc

libonlspinmon_client_la_SOURCES = \
  SpinMonDraw.cc

bin_SCRIPTS = \
  SpinMonSetup.csh \
  SpinMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlspinmon_server.la

testexternals_client_LDADD = \
  libonlspinmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
