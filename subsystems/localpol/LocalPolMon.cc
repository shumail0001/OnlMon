// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "LocalPolMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>
#include <Event/eventReceiverClient.h>


#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>

#include <cmath>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits

LocalPolMon::LocalPolMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new LocalPolMon()
  return;//test
}

LocalPolMon::~LocalPolMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int LocalPolMon::Init()
{

  h_example = new TH1D("h_example","h_example",100,0,100);
  h_example2 = new TH1D("h_example2","h_example2",100,0,100);

  OnlMonServer *se = OnlMonServer::instance();
  se->registerHisto(this, h_example);

  Reset();
  return 0;
} 

int LocalPolMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int LocalPolMon::process_event(Event *e /* evt */)
{
  
  Packet* p = e->getPacket(packetid);
  if (p){

  }

  return 0;

}

int LocalPolMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}
