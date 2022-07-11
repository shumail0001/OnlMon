AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir) -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libpoms.la   

libpoms_la_LIBADD = -L$(libdir) -lonlmonclient

libpoms_la_SOURCES = \
  Poms.cc

Poms_Dict.C: Poms.h PomsLinkDef.h
	rootcint -f $@ -c $(DEFS) $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict.*
