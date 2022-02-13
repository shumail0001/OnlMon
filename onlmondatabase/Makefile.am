AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS = -L$(libdir)

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_SPHENIX)/include \
  -I$(ROOTSYS)/include 

BUILT_SOURCES = \
  testexternals.C

include_HEADERS = \
  OnCalDBodbc.h \
  OnlMonDB.h \
  OnlMonDBodbc.h \
  OnlMonDBReturnCodes.h \
  RunDBodbc.h

lib_LTLIBRARIES = \
  libonlmonodbc.la \
  libonlmondb.la

libonlmondb_la_SOURCES = \
  OnlMonDB.C \
  OnlMonDBVar.C

libonlmonodbc_la_SOURCES = \
  OnCalDBodbc.C \
  OnlMonDBodbc.C \
  OnlMonDBodbc_Dict.C \
  RunDBodbc.C

libonlmondb_la_LIBADD = \
  libonlmonodbc.la \
  -lonlmonserver

libonlmonodbc_la_LIBADD = \
  -L$(OPT_SPHENIX)/lib \
  -lonlmonserver \
  -lodbc \
  -lodbc++

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libonlmondb.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

OnlMonDBodbc_Dict.C: OnCalDBodbc.h OnlMonDBodbc.h RunDBodbc.h OnlMonDBodbcLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
