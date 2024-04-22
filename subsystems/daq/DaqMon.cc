// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "DaqMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
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

enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

DaqMon::DaqMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new DaqMon()

  return;
}

DaqMon::~DaqMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int DaqMon::Init()
{
  gRandom->SetSeed(rand());
  // read our calibrations from DaqMonData.dat
  const char *daqcalib = getenv("DAQCALIB");
  if (!daqcalib)
  {
    std::cout << "DAQCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(daqcalib) + "/" + "DaqMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  h_gl1_clock_diff = new TH2D("h_gl1_clock_diff","", 6, 0,6, 2, -0.5, 1.5);
  h_gl1_clock_diff_capture = new TH2D("h_gl1_clock_diff_capture","", 200, 0, nEventsCapture, 6, 0, 6);
  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, h_gl1_clock_diff);  
  se->registerHisto(this, h_gl1_clock_diff_capture); 
  Reset();
  erc = new eventReceiverClient("gl1daq");
  return 0;
}

int DaqMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int DaqMon::process_event(Event *e /* evt */)
{
    if (e->getEvtType() >= 8)  /// special event where we do not read out the calorimeters
    {
        return 0;
    }

  evtcnt++;
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("daqhist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  
  if(gevtcnt<nEventsCapture){
      binindex = ((float) gevtcnt)/nEventsCapture*200+1;
      gevtcnt++;
      if(gevtcnt==nEventsCapture) gevtcnt=0; 
  }
  uint64_t gl1_clock = 0;
  int evtnr = e->getEvtSequence();
  Event *gl1Event = erc->getEvent(evtnr);
  if (gl1Event){
      Packet* p = gl1Event->getPacket(14001);
      if (p){
          gl1_clock = p->lValue(0,"BCO");
      }   
  }
 
  Packet * plist[100];
  int npackets = e->getPacketList(plist,100);
  for (int ipacket = 0; ipacket < npackets; ipacket++) {
      Packet * p = plist[ipacket];
      if (p != nullptr) {
          int pnum = p->getIdentifier();
          int calomapid = CaloPacketMap(pnum);
          uint32_t packet_clock = p->iValue(0,"CLOCK");
          clockdiff = (gl1_clock & 0xFFFFFFFF) - packet_clock;
          uint32_t diff = clockdiff - previousdiff;
          bool fdiff = (diff == 0) ? true : false;
          if(gevtcnt>0){
              if(binindex>previndex || fdiff==false){
                  h_gl1_clock_diff_capture->SetBinContent(binindex,calomapid+1,fdiff);
              }
          }
          if(previousdiff!=0) h_gl1_clock_diff->Fill(calomapid,fdiff);
          previndex = binindex;
          previousdiff = clockdiff;
      }
      delete p;
  }


  return 0;
}

int DaqMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int DaqMon::CaloPacketMap(int pnum)
{
    int caloid = -1;
    if(pnum>=packet_mbd_low && pnum<=packet_mbd_high) caloid=0;
    else if(pnum>=packet_emcal_low && pnum<=packet_emcal_high) caloid=1;
    else if(pnum>=packet_ihcal_low && pnum<=packet_ihcal_high) caloid=2;
    else if(pnum>=packet_ohcal_low && pnum<=packet_ohcal_high) caloid=3;
    else if(pnum>=packet_sepd_low && pnum<=packet_sepd_high) caloid=4;
    else if(pnum>=packet_zdc) caloid=5;
    return caloid;
}
