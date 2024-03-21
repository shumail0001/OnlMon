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
	
  OnlMonServer *se = OnlMonServer::instance();
  
  const char* zdccalib=getenv("ZDCCALIB");
  if(!zdccalib){
    std::cout<<"ZDCCALIB environment variable not set"<<std:endl;
    exit(1);
  }
  std::string calibfilename=std::string(zdccalib)+"/"+"SMDRelativeGain.dat";
  std::ifstream calibinput(calibfilename);
  std::ostringstream msg(calib);

  if(!calibinput){
    msg<<calibinput<<" could not be openeds.";
    se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL,msg.str(),2);
    exit(1);
  }
  int NS;
  int channel;
  float gain;
  for(int ch=0; ch<32; ch++){
    calibinput>>NS>>channel>>gain;
    
    if(NS==0 && channel>=0 && channel <16)smd_north_rgain[channel]=gain;
    else if(NS==1 &&channel>=0 && channel <16) smd_south_rgain[channel]=gain;
    else {
      msg<<calibinput<< "could not understand the content, expect int(0/1) int(0-15) float for North or South, channel number and relative gain";
      se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL,msg.str(),2);
      exit(1);
    }
  }
  
  h_Counts         = new TH1D*[4];
  h_CountsScramble = new TH1D*[4];
  
  h_Counts[0]         = new TH1D("h_BlueCountsUD","h_BlueCountsUD",4,0,4);
  h_Counts[1]         = new TH1D("h_BlueCountsLR","h_BlueCountsLR",4,0,4);
  h_Counts[2]         = new TH1D("h_YellCountsUD","h_YellCountsUD",4,0,4);
  h_Counts[3]         = new TH1D("h_YellCountsLR","h_YellCountsLR",4,0,4);
  h_CountsScramble[0] = new TH1D("h_BlueCountsScrambleUD","h_BlueCountsScrambleUD",4,0,4);
  h_CountsScramble[1] = new TH1D("h_BlueCountsScrambleLR","h_BlueCountsScrambleLR",4,0,4);
  h_CountsScramble[2] = new TH1D("h_YellCountsScrambleUD","h_YellCountsScrambleUD",4,0,4);
  h_CountsScramble[3] = new TH1D("h_YellCountsScrambleLR","h_YellCountsScrambleLR",4,0,4);

  se->registerHisto(this, h_Counts        [0]);
  se->registerHisto(this, h_Counts        [1]);
  se->registerHisto(this, h_Counts        [2]);
  se->registerHisto(this, h_Counts        [3]);
  se->registerHisto(this, h_CountsScramble[0]);
  se->registerHisto(this, h_CountsScramble[1]);
  se->registerHisto(this, h_CountsScramble[2]);
  se->registerHisto(this, h_CountsScramble[3]);

  WaveformProcessingFast = new CaloWaveformFitting();

  Reset();
  return 0;
} 

int LocalPolMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
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
  
  if(e->getEvtType() == BEGRUNEVENT/*9*/){ //spin patterns stored in BeginRun event
    bool first=true;
    Packet* bluepacket = e->getPacket(14902);
    Packet* yellpacket = e->getPacket(14903);
    if(bluepacket && yellpacket){
      for(int i = 0; i< 120; i++){
	SpinPatterns[BLUE][i]=bluepacket->iValue(i);
	SpinPatterns[YELLOW][i]=yellpacket->iValue(i);
	if(first && (true /*abort gap def from HTML*/)){
	  first=false;
	  StartAbortGapPattern=i;
	}
      }
    }
    //delete bluepacket;
    //delete yellpacket;
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
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i]->Fill(1);//Left for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i]->Fill(1);//Left for pointing down
      }
      else{
	// (i/2)=0 for blue beam, =1 for yellow beam   
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i]->Fill(2);//Right for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i]->Fill(2);//Right for pointing down
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
