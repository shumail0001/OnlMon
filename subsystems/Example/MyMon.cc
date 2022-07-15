// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)
#include "MyMon.h"
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <Event/Event.h>
#pragma GCC diagnostic pop

#include <Event/msg_control.h>
#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

MyMon::MyMon(const char *name)
  : OnlMon(name)
  , evtcnt(0)
  , idummy(0)
  , dbvars(NULL)
  , myhist1(NULL)
  , myhist2(NULL)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new MyMon()
  return;
}

MyMon::~MyMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  delete dbvars;
  return;
}

int MyMon::Init()
{
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  myhist1 = new TH1F("mymon_hist1", "test 1d histo", 101, 0., 100.);
  myhist2 = new TH2F("mymon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);
  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, myhist1);  // uses the TH1->GetName() as key
  se->registerHisto(this, myhist2);
  //  dbvars = new OnlMonDB(ThisName); // use monitor name for db table name
  //  DBVarInit();
  Reset();
  return 0;
}

int MyMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int MyMon::process_event(Event * /* evt */)
{
  evtcnt++;
  OnlMonServer *se = OnlMonServer::instance();
  // using ONLMONBBCLL1 makes this trigger selection configurable from the outside
  // e.g. if the BBCLL1 has problems or if it changes its name
  if (!se->Trigger("ONLMONBBCLL1"))
  {
    ostringstream msg;
    msg << "Processing Event " << evtcnt
        << ", Trigger : 0x" << hex << se->Trigger()
        << dec;
    // severity levels and id's for message sources can be found in
    // $ONLINE_MAIN/include/msg_profile.h
    // The last argument is a message type. Messages of the same type
    // are throttled together, so distinct messages should get distinct
    // message types
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), TRGMESSAGE);
  }
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("myhist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  myhist1->Fill((float) idummy);
  myhist2->Fill((float) idummy, (float) idummy, 1.);

  if (idummy++ > 1000)
  {
    if (dbvars)
    {
      dbvars->SetVar("mymoncount", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("mymondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("mymonnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
  }
  return 0;
}

int MyMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int MyMon::DBVarInit()
{
  // variable names are not case sensitive
  string varname;
  varname = "mymoncount";
  dbvars->registerVar(varname);
  varname = "mymondummy";
  dbvars->registerVar(varname);
  varname = "mymonnew";
  dbvars->registerVar(varname);
  if (verbosity > 0)
  {
    dbvars->Print();
  }
  dbvars->DBInit();
  return 0;
}
