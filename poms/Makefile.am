AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libpoms.la   

pkginclude_HEADERS = \
  Poms.h

libpoms_la_LIBADD = \
  -L$(libdir) \
  -lonlmonclient

libpoms_la_SOURCES = \
  Poms.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libpoms.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


clean-local:
	rm -f $(BUILT_SOURCES)
