// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "LocalPolMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>
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
#include <map>
#include <utility>

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

  h_Counts         = new TH1D*[2];
  h_CountsScramble = new TH1D*[2];
  
  h_Counts[0]         = new TH1D("h_BlueCounts","h_BlueCounts",4,0,4);
  h_Counts[1]         = new TH1D("h_YellCounts","h_YellCounts",4,0,4);
  h_CountsScramble[0] = new TH1D("h_BlueCountsScramble","h_BlueCountsScramble",4,0,4);
  h_CountsScramble[1] = new TH1D("h_YellCountsScramble","h_YellCountsScramble",4,0,4);
  
  OnlMonServer *se = OnlMonServer::instance();

  se->registerHisto(this, h_Counts        [0]);
  se->registerHisto(this, h_Counts        [1]);
  se->registerHisto(this, h_CountsScramble[0]);
  se->registerHisto(this, h_CountsScramble[1]);
    
  WaveformProcessingFast = new CaloWaveformFitting();

  Reset();
  return 0;
} 

int LocalPolMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  //To be implemented with the right values from calibration files
  for(int i=0; i<16; i++){
    smd_north_rgain[i]=1.;
    smd_south_rgain[i]=1.;
  }
  
  return 0;
}


float LocalPolMon::anaWaveformFast(Packet *p, const int channel)
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
  return result.at(0);
  //return result;
}

int LocalPolMon::process_event(Event *e /* evt */)
{
  
  if(e->getEvtType() == 9){ //spin patterns stored in BeginRun event
    bool first=true;
    for(int i = 0; i< 120; i++){
      SpinPatterns[BLUE][i]=0;
      SpinPatterns[YELLOW][i]=0;
      if(first && (true /*abort gap def from HTML*/)){
	first=false;
	StartAbortGapPattern=i;
      }
    }
  }
  Packet* pgl1 = e->getPacket(packetid_gl1);
  //Here we do the magic to identify the abort gap
  std::map<int, int> gl1_counter[16];
  std::vector<int> gap[16];
  int begingap[16];
  int endgap[16];
  if(pgl1){
    //taken from SpinMon.cc
    int bunchnr = pgl1->lValue(0,"BunchNumber");
    for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
      int counts = pgl1->lValue(i,2); 
      //update instead of add
      gl1_counter[i][bunchnr]=counts; 
    } 
  }

  //Here we do the matching to identify the crossing shift
  if(evtcnt>120){//Do we need to somehow wait or from the first events the scalers for all bunches would be already filled?
    for(int i=0; i<16; i++){
      for(int emptyfill=0; emptyfill<9; emptyfill++){
	gap[i].push_back(min_element(gl1_counter[i].begin(),gl1_counter[i].end(),[](const std::pair<int,int>&lhs, const std::pair<int,int>& rhs){return lhs.second<rhs.second;})->first);
	gl1_counter[i].erase(gap[i].back());
      }
      begingap[i]=*min_element(gap[i].begin(), gap[i].end());
      endgap[i]  =*max_element(gap[i].begin(), gap[i].end());
      if(endgap[i]-begingap[i]>9) std::cout<<" Weird abort gap is larger than 9 bunches "<<std::endl;
    }
    for(int i=1; i<16; i++){
      if(begingap[0]!=begingap[i]) std::cout<<" Weird abort gap not in the same location between trigger bit 0 and trigger bit "<<i<<std::endl;
    }
    StartAbortGapData=begingap[0];
  }
  CrossingShift=StartAbortGapPattern-StartAbortGapData;
  Packet* psmd = e->getPacket(packetid_smd);
  float smd_adc[32];
  if (psmd && pgl1){
    int bunchnr = pgl1->lValue(0,"BunchNumber");
    for(int ch=16; ch<psmd->iValue(0,"CHANNELS"); ch++){
      //First 16 channels are for the ZDC, the next channels are for the SMD
      //float signalFast = anaWaveformFast(psmd, ch);  // fast waveform fitting
      float signalFast = anaWaveformFast(psmd, ch);  // fast waveform fitting
      //float signalFast = resultFast.at(0);

      //Scale according to relative gain calibration factor
      if(ch<32) smd_adc[ch-16]=signalFast*smd_north_rgain[ch-16];
      else smd_adc[ch-16]=signalFast*smd_south_rgain[ch-32];
    }

    float Weights[4]={0};
    memset(Weights, 0, sizeof(Weights));
    float AveragePosition[4]={0};
    memset(AveragePosition,0,sizeof(AveragePosition));

    for(int ch=0; ch<8; ch++){
      Weights[0]         += smd_adc[ch];
      AveragePosition[0] += pitchY*(ch-nchannelsY/2)*smd_adc[ch];//North Y direction (Asym in blue)
      Weights[2]         += smd_adc[ch+16];
      AveragePosition[2] += pitchY*(ch-nchannelsY/2)*smd_adc[ch+16];//South Y direction (Asym in Yellow)

      if(ch==7) continue;
      Weights[1]         += smd_adc[ch+8];
      AveragePosition[1] += pitchX*(ch-nchannelsX/2)*smd_adc[ch+8];//North X direction (Asym in Blue)
      Weights[3]         += smd_adc[ch+24];
      AveragePosition[3] += pitchX*(ch-nchannelsX/2)*smd_adc[ch+24];//South X direction (Asym in Yellow)
    }

    for(int i=0; i<4; i++){
      if(Weights[i]>0.0) AveragePosition[i]/=Weights[i];
      else AveragePosition[i]=0.;
      
      if(AveragePosition[i]<0){
	// (i/2)=0 for blue beam, =1 for yellow beam
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i/2]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i/2]->Fill(1);//Left for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i/2]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i/2]->Fill(1);//Left for pointing down
      }
      else{
	// (i/2)=0 for blue beam, =1 for yellow beam   
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i/2]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i/2]->Fill(4);//Right for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i/2]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i/2]->Fill(4);//Right for pointing down
      }
    }
  }

  return 0;

}

int LocalPolMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}
