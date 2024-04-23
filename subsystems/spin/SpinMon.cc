// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "SpinMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <xingshiftcal/XingShiftCal.h>

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

  hspinpatternBlue = new TH1I("h1_spinpatternBlue","",120,-0.5,119.5);
  hspinpatternYellow = new TH1I("h1_spinpatternYellow","",120,-0.5,119.5);

  pCspin_patternBlueUp = new TH2I("h2_pCspinpatternBlueUp","",120,-0.5,119.5,2,0.5,2.5);
  pCspin_patternBlueDown = new TH2I("h2_pCspinpatternBlueDown","",120,-0.5,119.5,2,0.5,2.5);
  pCspin_patternBlueUnpol = new TH2I("h2_pCspinpatternBlueUnpol","",120,-0.5,119.5,2,0.5,2.5);

  pCspin_patternYellowUp = new TH2I("h2_pCspinpatternYellowUp","",120,-0.5,119.5,2,0.5,2.5);
  pCspin_patternYellowDown = new TH2I("h2_pCspinpatternYellowDown","",120,-0.5,119.5,2,0.5,2.5);
  pCspin_patternYellowUnpol = new TH2I("h2_pCspinpatternYellowUnpol","",120,-0.5,119.5,2,0.5,2.5);

  spin_patternBlueUp = new TH2I("h2_spinpatternBlueUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueDown = new TH2I("h2_spinpatternBlueDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternBlueUnpol = new TH2I("h2_spinpatternBlueUnpol","",120,-0.5,119.5,2,0.5,2.5);

  spin_patternYellowUp = new TH2I("h2_spinpatternYellowUp","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowDown = new TH2I("h2_spinpatternYellowDown","",120,-0.5,119.5,2,0.5,2.5);
  spin_patternYellowUnpol = new TH2I("h2_spinpatternYellowUnpol","",120,-0.5,119.5,2,0.5,2.5);


  hpolBlue = new TH1D("h1_polBlue","",1,0,1);
  hpolYellow = new TH1D("h1_polYellow","",1,0,1);

  hxingshift = new TH1I("h1_xingshift","",2,0,2);


  for (int i = 0; i < NTRIG; i++)
  {
    gl1_counter[i] = new TH1I(Form("gl1_counter_trig%d",i),Form("gl1p_trig%d",i),120,-0.5,119.5);
  }

  for (auto &scalercount : scalercounts)
  {
    for (unsigned long &j : scalercount)
    {
      j = 0;
    }
  }


  OnlMonServer *se = OnlMonServer::instance();
  //se->registerHisto(this, hspinpatternBlue);
  //se->registerHisto(this, hspinpatternYellow);
  se->registerHisto(this, pCspin_patternBlueUp);
  se->registerHisto(this, pCspin_patternBlueDown);
  se->registerHisto(this, pCspin_patternBlueUnpol);
  se->registerHisto(this, pCspin_patternYellowUp);
  se->registerHisto(this, pCspin_patternYellowDown);
  se->registerHisto(this, pCspin_patternYellowUnpol);

  se->registerHisto(this, spin_patternBlueUp);
  se->registerHisto(this, spin_patternBlueDown);
  se->registerHisto(this, spin_patternBlueUnpol);
  se->registerHisto(this, spin_patternYellowUp);
  se->registerHisto(this, spin_patternYellowDown);
  se->registerHisto(this, spin_patternYellowUnpol);
  se->registerHisto(this, hpolBlue);
  se->registerHisto(this, hpolYellow);
  se->registerHisto(this, hxingshift);

  for (int i = 0; i < NTRIG; i++)
  {
    se->registerHisto(this, gl1_counter[i]);
  }

  Reset();

  XingShift = new XingShiftCal();

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
  
  if (e->getEvtType() == 9) //spin patterns stored in BeginRun event (9)
  {

    hxingshift->SetBinContent(1,defaultxingshift);

    pBlueSpin = e->getPacket(packet_BLUESPIN);
    pYellSpin = e->getPacket(packet_YELLSPIN);

    //pBluePol = e->getPacket(packet_BLUEPOL);
    //pYellPol = e->getPacket(packet_YELLPOL);

    //pBlueAsym = e->getPacket(packet_BLUEASYM);
    //pYellAsym = e->getPacket(packet_YELLASYM);

    //pBlueIntPattern = e->getPacket(packet_BLUEINTPATTERN);
    //pYellIntPattern = e->getPacket(packet_YELLINTPATTERN);
    //pBluePolPattern = e->getPacket(packet_BLUEPOLPATTERN);
    //pYellPolPattern = e->getPacket(packet_YELLPOLPATTERN);
    

    //Fill beam polarization histograms
    //float polBlue = pBluePol->iValue(0);
    //float polBlueErr = pBluePol->iValue(1);
    float polBlue = 1.;
    float polBlueErr = 0.1;
    hpolBlue->SetBinContent(1,polBlue);
    hpolBlue->SetBinError(1,polBlueErr);

    //float polYellow = pYellowPol->iValue(0);
    //float polYellowErr = pYellowPol->iValue(1);
    float polYellow = 1.;
    float polYellowErr = 0.1;
    hpolYellow->SetBinContent(1,polYellow);
    hpolYellow->SetBinError(1,polYellowErr);

    // up = 1
    // down = -1
    // 0 for abort gap. bunches 111 -> 120
    for (int i = 0; i < NBUNCHES; i++)
    {
      if (i > 110) 
      {
        blueSpinPattern[i] = -10;
        yellSpinPattern[i] = -10;
      }
      else
      {
        blueSpinPattern[i] = pBlueSpin->iValue(i);
        yellSpinPattern[i] = pYellSpin->iValue(i);

        if (blueSpinPattern[i] == 1){spin_patternBlueUp->Fill(i,1);}
        if (blueSpinPattern[i] == -1){spin_patternBlueDown->Fill(i,1);}
        if (blueSpinPattern[i] == 0){spin_patternBlueUnpol->Fill(i,1);}

        if (yellSpinPattern[i] == 1){spin_patternYellowUp->Fill(i,2);}
        if (yellSpinPattern[i] == -1){spin_patternYellowDown->Fill(i,2);}
        if (yellSpinPattern[i] == 0){spin_patternYellowUnpol->Fill(i,2);}
      }  
    }
    
    delete pBlueSpin;
    delete pYellSpin;

    /*
    //Get bunch asymmetries for measured spin pattern
    //there are 360 buckets
    for (int i = 0; i < 360; i++)
    { 
      blueSpinPattern[i] = pBlueSpin->iValue(i);
      yellSpinPattern[i] = pYellSpin->iValue(i);

      if (blueSpinPattern[i] == 1){spin_patternBlueUp->Fill(i,2);}
      if (blueSpinPattern[i] == -1){spin_patternBlueDown->Fill(i,2);}
      if (blueSpinPattern[i] == 0){spin_patternBlueUnpol->Fill(i,2);}

      if (yellSpinPattern[i] == 1){spin_patternYellowUp->Fill(i,2);}
      if (yellSpinPattern[i] == -1){spin_patternYellowDown->Fill(i,2);}
      if (yellSpinPattern[i] == 0){spin_patternYellowUnpol->Fill(i,2);}
    }
    */


  }

  else if (e->getEvtType() == 1)
  {
  //******** gl1p scalers *********//
    int evtnr = e->getEvtSequence();
    Event *gl1Event = erc->getEvent(evtnr);
    if (gl1Event)
    {
      Packet* p = gl1Event->getPacket(packetid_GL1);
      if (p)
      {
	//int triggervec = p->lValue(0,"TriggerVector");
	int bunchnr = (p->lValue(0,"BunchNumber") + defaultxingshift) % NBUNCHES;
	for (int i = 0; i < 16; i++) 
        { 
	  //2nd arg of lValue: 0 is raw trigger count, 1 is live trigger count, 2 is scaled trigger count
	  int counts = p->lValue(i,"GL1PLIVE"); //live gl1p cnts. 
	  //update instead of add
	  gl1_counter[i]->SetBinContent(bunchnr+1,counts); //update bin with new scaler info. instead of adding every evt
	  scalercounts[i][bunchnr] = counts;
	}
      }
    }
  
    if (!success && evtcnt > 4999 && evtcnt % 5000 == 0)
    {
      XingShift->CalculateCrossingShift(xingshift, scalercounts, success);
      int addxingshift = 0;
      if (success)
      {
	addxingshift = xingshift;
      }
      hxingshift->SetBinContent(2,addxingshift);
    }
  }

  return 0;

}

int SpinMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}
