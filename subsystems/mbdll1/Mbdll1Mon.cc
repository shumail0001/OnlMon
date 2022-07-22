// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "Mbdll1Mon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>

#include <cmath>
#include <cstdio>  // for printf
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

Mbdll1Mon::Mbdll1Mon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new Mbdll1Mon()
  return;
}

Mbdll1Mon::~Mbdll1Mon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  delete dbvars;
  return;
}

int Mbdll1Mon::Init()
{
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  mbdll1hist1 = new TH1F("mbdll1mon_hist1", "test 1d histo", 101, 0., 100.);
  mbdll1hist2 = new TH2F("mbdll1mon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);
  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, mbdll1hist1);  // uses the TH1->GetName() as key
  se->registerHisto(this, mbdll1hist2);
  dbvars = new OnlMonDB(ThisName);  // use monitor name for db table name
  DBVarInit();
  Reset();
  return 0;
}

int Mbdll1Mon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int Mbdll1Mon::process_event(Event * /* evt */)
{
  evtcnt++;
  OnlMonServer *se = OnlMonServer::instance();
  // using ONLMONBBCLL1 makes this trigger selection configurable from the outside
  // e.g. if the BBCLL1 has problems or if it changes its name
  if (!se->Trigger("ONLMONBBCLL1"))
  {
    std::ostringstream msg;
    msg << "Processing Event " << evtcnt
        << ", Trigger : 0x" << std::hex << se->Trigger()
        << std::dec;
    // severity levels and id's for message sources can be found in
    // $ONLINE_MAIN/include/msg_profile.h
    // The last argument is a message type. Messages of the same type
    // are throttled together, so distinct messages should get distinct
    // message types
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), TRGMESSAGE);
  }
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("mbdll1hist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  mbdll1hist1->Fill((float) idummy);
  mbdll1hist2->Fill((float) idummy, (float) idummy, 1.);

  if (idummy++ > 10)
  {
    if (dbvars)
    {
      dbvars->SetVar("mbdll1moncount", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("mbdll1mondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("mbdll1monnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    std::ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
  }
  return 0;
}

int Mbdll1Mon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int Mbdll1Mon::DBVarInit()
{
  // variable names are not case sensitive
  std::string varname;
  varname = "mbdll1moncount";
  dbvars->registerVar(varname);
  varname = "mbdll1mondummy";
  dbvars->registerVar(varname);
  varname = "mbdll1monnew";
  dbvars->registerVar(varname);
  if (verbosity > 0)
  {
    dbvars->Print();
  }
  dbvars->DBInit();
  return 0;
}
