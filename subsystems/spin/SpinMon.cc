// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "SpinMon.h"

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

SpinMon::SpinMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new SpinMon()
  return;
}

SpinMon::~SpinMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int SpinMon::Init()
{

  spin_patternBlueUp = new TH2I("h2_spinpatternBlueUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueDown = new TH2I("h2_spinpatternBlueDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueUnpol = new TH2I("h2_spinpatternBlueUnpol","",120,-0.5,119.5,2,0.5,2.5);

  spin_patternYellowUp = new TH2I("h2_spinpatternYellowUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowDown = new TH2I("h2_spinpatternYellowDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowUnpol = new TH2I("h2_spinpatternYellowUnpol","",120,-0.5,119.5,2,0.5,2.5);

  for (int i = 0; i < NTRIG; i++){
    gl1_counter[i] = new TH1I(Form("gl1_counter_trig%d",i),Form("gl1_counter_trig%d",i),120,-0.5,119.5);
  }


  OnlMonServer *se = OnlMonServer::instance();
  se->registerHisto(this, spin_patternBlueUp);
  se->registerHisto(this, spin_patternBlueDown);
  se->registerHisto(this, spin_patternBlueUnpol);
  se->registerHisto(this, spin_patternYellowUp);
  se->registerHisto(this, spin_patternYellowDown);
  se->registerHisto(this, spin_patternYellowUnpol);

  for (int i = 0; i < NTRIG; i++){
    se->registerHisto(this, gl1_counter[i]);
  }

  Reset();

  erc = new eventReceiverClient("gl1daq");
 
  return 0;
} 

int SpinMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int SpinMon::process_event(Event *e /* evt */)
{
  evtcnt++;
  //******* Spin patterns from Martin html delivery *******//
  //******* from BeginRun event *************
  
  if (e->getEvtType() == 9){ //spin patterns stored in BeginRun event
    for (int i = 0; i < 120; i++){
      //******* placeholder until html delivery from C-AD is available ***********
      int blue_up = 0;
      int blue_down = 0;
      int blue_unpol = 0;
      int yellow_up = 0;
      int yellow_down = 0;
      int yellow_unpol = 0;

      if (i % 2 == 0){blue_up = 1;}
      else{blue_down = 1;}
      if (int(0.5*i) % 2 == 0){yellow_up = 1;}
      else {yellow_down = 1;}
      // **************************************************************************
      
      if (i < 111){
	if (blue_up){spin_patternBlueUp->Fill(i,2);}
	if (blue_down){spin_patternBlueDown->Fill(i,2);}
	if (blue_unpol){spin_patternBlueUnpol->Fill(i,2);}

	if (yellow_up){spin_patternYellowUp->Fill(i,1);}
	if (yellow_down){spin_patternYellowDown->Fill(i,1);}
	if (yellow_unpol){spin_patternYellowUnpol->Fill(i,1);}
      }

    }
  }


    //******** gl1p scalers *********//

  //int evtnr = e->getEvtSequence();
  //Event *gl1Event = erc->getEvent(evtnr);
  //if (gl1Event){
    Packet* p = e->getPacket(packetid_GL1);
    if (p){
      //int triggervec = p->lValue(0,"TriggerVector");
      int bunchnr = p->lValue(0,"BunchNumber");
      for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
        int counts = p->lValue(i,2); //scaled gl1 cnts. (change to cnts. on bunchnr crossng when implemented) 
	//update instead of add
        gl1_counter[i]->SetBinContent(bunchnr+1,counts); //update bin with new scaler info. instead of adding every evt
      }
    }
    //}
  
  return 0;

}

int SpinMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}
