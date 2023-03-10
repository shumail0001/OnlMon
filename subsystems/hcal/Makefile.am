AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES = \
  testexternals.cc

SUBDIRS = calib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libonlhcalmon_client.la \
  libonlhcalmon_server.la

libonlhcalmon_server_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonserver \
  -lonlmondb \
  -lonlmonutils

libonlhcalmon_client_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lonlmonclient \
  -lonlmondb


hcalincludedir=$(pkgincludedir)/hcal

hcalinclude_HEADERS = \
  HcalMon.h \
  HcalMonDraw.h

libonlhcalmon_server_la_SOURCES = \
  HcalMon.cc

libonlhcalmon_client_la_SOURCES = \
  HcalMonDraw.cc

bin_SCRIPTS = \
  HcalMonSetup.csh \
  HcalMonSetup.sh

noinst_PROGRAMS = \
  testexternals_server \
  testexternals_client


testexternals_server_SOURCES = \
  testexternals.cc

testexternals_client_SOURCES = \
  testexternals.cc

testexternals_server_LDADD = \
  libonlhcalmon_server.la

testexternals_client_LDADD = \
  libonlhcalmon_client.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
