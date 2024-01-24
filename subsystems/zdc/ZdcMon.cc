// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "ZdcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>
#include <calobase/TowerInfoDefs.h>
#include <caloreco/CaloWaveformFitting.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>


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

ZdcMon::ZdcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new ZdcMon()
  return;
}

ZdcMon::~ZdcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int ZdcMon::Init()
{
  const float MAX_ENERGY1 = 6500.;
  const float MAX_ENERGY2 = 6500.;
  const int BIN_NUMBER = 300;
    
  // read our calibrations from ZdcMonData.dat
  const char *zdccalib = getenv("ZDCCALIB");
  if (!zdccalib)
  {
    std::cout << "ZDCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(zdccalib) + "/" + "ZdcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  zdc_adc_north = new TH1F("zdc_adc_north", "ZDC ADC north", BIN_NUMBER, 0, MAX_ENERGY1);
  zdc_adc_south = new TH1F("zdc_adc_south", "ZDC ADC south", BIN_NUMBER, 0, MAX_ENERGY2);

  OnlMonServer *se = OnlMonServer::instance();
  //register histograms with server otherwise client won't get them
  se->registerHisto(this, zdc_adc_north );
  se->registerHisto(this, zdc_adc_south );
    
  WaveformProcessingFast = new CaloWaveformFitting();
  
  Reset();
  return 0;
}

int ZdcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

std::vector<float> ZdcMon::anaWaveformFast(Packet *p, const int channel)
{
  std::vector<float> waveform;
  for (int s = 0; s < p->iValue(0, "SAMPLES"); s++)
  {
    waveform.push_back(p->iValue(s, channel));
  }
  std::vector<std::vector<float>> multiple_wfs;
  multiple_wfs.push_back(waveform);

  std::vector<std::vector<float>> fitresults_zdc;
  fitresults_zdc = WaveformProcessingFast->calo_processing_fast(multiple_wfs);

  std::vector<float> result;
  result = fitresults_zdc.at(0);

  return result;
}


int ZdcMon::process_event(Event *e /* evt */)
{
  evtcnt++;
 
    int packet = 12001;
    Packet *p = e->getPacket(packet);
    if (p)
    {
    
      for (int c = 0; c < p->iValue(0, "CHANNELS"); c++)
      {
      
        std::vector<float> resultFast = anaWaveformFast(p, c);  // fast waveform fitting
        float signalFast = resultFast.at(0);
        float signal = signalFast;
          
        unsigned int towerkey = TowerInfoDefs::decode_zdc(c);
        int zdc_side = TowerInfoDefs::get_zdc_side(towerkey);
        if(c < 16)
        {
         if (zdc_side == 0)
         {
            zdc_adc_south->Fill(signal);
         }
         else if (zdc_side == 1)
         {
            zdc_adc_north->Fill(signal);
         }
         else
         {
           std::cout << "arm bin not assigned ... " << std::endl;
           return -1;
         }
        }  //select zdc channels only
       }  // channel loop end
     }   //  if packet good

    delete p;

  return 0;
}

int ZdcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

