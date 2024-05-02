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
  h_gl1_clock_diff->GetXaxis()->SetTitleSize(0);
  h_gl1_clock_diff->GetYaxis()->SetNdivisions(202);
  h_gl1_clock_diff->GetXaxis()->SetNdivisions(101);
  h_gl1_clock_diff->GetYaxis()->SetBinLabel(1,"#bf{Unlocked}");
  h_gl1_clock_diff->GetYaxis()->SetBinLabel(2,"#bf{Locked}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(1,"#bf{MBD}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(2,"#bf{EMCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(3,"#bf{IHCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(4,"#bf{OHCal}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(5,"#bf{sEPD}");
  h_gl1_clock_diff->GetXaxis()->SetBinLabel(6,"#bf{ZDC}");
  h_gl1_clock_diff_capture->GetXaxis()->SetTitle("Latest 1M events");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(1,"#bf{MBD}");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(2,"#bf{EMCal}");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(3,"#bf{IHCal}");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(4,"#bf{OHCal}");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(5,"#bf{sEPD}");
  h_gl1_clock_diff_capture->GetYaxis()->SetBinLabel(6,"#bf{ZDC}");
 
  int nbins = packet_zdc-packet_mbd_low + 1;
  h_unlock_hist = new TH1D("h_unlock_hist",";packet number;",nbins,packet_mbd_low-0.5,packet_zdc+0.5);

  h_unlock_clock = new TH2F("h_unlock_clock",";Type;Clock",2,0,2,42949672,0,42949672);

  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, h_gl1_clock_diff);  
  se->registerHisto(this, h_gl1_clock_diff_capture); 
  se->registerHisto(this, h_unlock_hist); 
  se->registerHisto(this, h_unlock_clock); 
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
  
  int evtnr = e->getEvtSequence();
  if(evtnr<3) return 0;

  evtcnt++;
  
  if(gevtcnt<nEventsCapture){
      binindex = ( gevtcnt*200)/nEventsCapture+1;
      gevtcnt++;
      if(gevtcnt==nEventsCapture) gevtcnt=0; 
  }
  long int gl1_clock = 0;
  Event *gl1Event = erc->getEvent(evtnr);

  if(!gl1Event) return 0;
  Packet* pgl1 = gl1Event->getPacket(14001);
  if (pgl1){
      gl1_clock = pgl1->lValue(0,"BCO");
  }   
 
  Packet * plist[100];
  int npackets = e->getPacketList(plist,100);
  for (int ipacket = 0; ipacket < npackets; ipacket++) {
      Packet * p = plist[ipacket];
      if (p != nullptr) {
          int pnum = p->getIdentifier();
          int calomapid = CaloPacketMap(pnum);
          long int packet_clock = p->iValue(0,"CLOCK");
          clockdiff[ipacket] = gl1_clock  - packet_clock;
          int fdiff = (clockdiff[ipacket] != previousdiff[ipacket]) ? 0 : 1;
          if(gevtcnt>1 && evtcnt>3){
              if(binindex>previndex || fdiff==0){
                  h_gl1_clock_diff_capture->SetBinContent(binindex,calomapid+1,fdiff);
              }
              previndex = binindex;
          }
          if(evtcnt>3) h_gl1_clock_diff->Fill(calomapid,fdiff);
          if(fdiff==0){
              h_unlock_hist ->Fill(pnum);
              h_unlock_clock -> Fill(0., gl1_clock);
              h_unlock_clock -> Fill(1., packet_clock);
          }

          previousdiff[ipacket] = clockdiff[ipacket];
      }
      delete p;
  }


  return 0;
}

int DaqMon::Reset()
{
  // reset our internal counters
  //evtcnt = 0;
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
