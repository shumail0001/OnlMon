AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include/Event \
  -I$(OPT_SPHENIX)/include \
  -isystem$(ROOTSYS)/include

RINCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ONLINE_MAIN)/include \
  -I$(OPT_SPHENIX)/include \
  -I$(ROOTSYS)/include

BUILT_SOURCES = \
  testexternals.cc

lib_LTLIBRARIES = \
  libonlmonserver.la \
  libonlmonserver_funcs.la

libonlmonserver_funcs_la_LIBADD = \
  libonlmonserver.la \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lphool \
  -lpmonitor \
  -lmessage

libonlmonserver_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lmessage \
  -lphool \
  -lodbc++


noinst_HEADERS = \
  pmonitorInterface.h

pkginclude_HEADERS = \
  HistoBinDefs.h \
  OnlMon.h \
  OnlMonBase.h \
  OnlMonServer.h \
  OnlMonStatus.h \
  OnlMonTrigger.h \
  PortNumber.h

libonlmonserver_funcs_la_SOURCES = \
  pmonitorInterface.cc

libonlmonserver_la_SOURCES = \
  MessageSystem.cc \
  OnlMon.cc \
  OnlMonBase.cc \
  OnlMonServer.cc \
  OnlMonStatusDB.cc

#  OnlMonTrigger.cc
#  TrigInfo.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libonlmonserver.la \
  libonlmonserver_funcs.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


OnlMonServer_Dict.C: \
  OnlMonServer.h \
  OnlMonStatusDB.h \
  OnlMonTrigger.h \
  OnlMonServerLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

OnlMon_Dict.C: OnlMon.h OnlMonBase.h OnlMonLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

clean-local:
	rm -f *Dict.*
