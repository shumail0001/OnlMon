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
#include <TString.h>

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
  const char* locpolcal=getenv("LOCALPOLCALIB");
  if(!locpolcal){
    std::cout<<"LOCALPOLCALIB environment variable not set"<<std::endl;
    exit(1);
  }
  std::string lpconfigfilename=std::string(locpolcal)+"/"+"LocPolConfig.dat";
  std::ifstream config(lpconfigfilename);
  std::ostringstream msg_config(lpconfigfilename);

  if(!config){
    msg_config<<lpconfigfilename<<" could not be openeds.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL,msg_config.str(),2);
    exit(1);
  }
  int err_counter=0;
  TString key;
  int  val;
  while(!config.eof()){
    config>>key>>val;
    if(!config.good())break;
    key.ToLower();
    if(key=="trigger"){
      for(int i=0; i<16; i++){
	goodtrigger[i]=false;
	if((val&(1<<i))){
	  goodtrigger[i]=true;
	}
      }
    }
    else if(key=="testfake"){
      if(val==1){
	fake=true;
      }
      else fake=false;
    }
    else if(key=="sphenixgap"){
      if(val>=0 && val<120){
	ExpectedsPhenixGapPosition=val; 
      }
    }
    else if(key=="threshold"){
      if(val>0&&val<1e5)EventCountThresholdGap=val;
    }
    else if(key=="verbosity"){
      if(val==1) {
	std::cout<<"Making it verbose"<<std::endl;
	verbosity=true;
      }
    }
    else{
      err_counter++;
      std::cout<<"Unknown configuration \n Expected: verbosity, threshold, sphenixgap, testfake or trigger key words"<<std::endl;
    }
    if(err_counter>3){
      std::cout<<"More than 3 unknown key words, abort"<<std::endl;
      exit(1);
    } 
  }

  
  const char* zdccalib=getenv("ZDCCALIB");
  if(!zdccalib){
    std::cout<<"ZDCCALIB environment variable not set"<<std::endl;
    exit(1);
  }
  std::string calibfilename=std::string(zdccalib)+"/"+"SMDRelativeGain.dat";
  std::ifstream calibinput(calibfilename);
  std::ostringstream msg(calibfilename);

  if(!calibinput){
    msg<<calibfilename<<" could not be openeds.";
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
      msg<<calibfilename<< "could not understand the content, expect int(0/1) int(0-15) float for North or South, channel number and relative gain";
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
  myRandomBunch=new TRandom(0);
  Reset();
  return 0;
} 

int LocalPolMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  //Initialisation of the map (hopefully this step will not be required when gl1p scalers become available
  for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
    for(int bunchnr=0; bunchnr<120; bunchnr++) gl1_counter[i][bunchnr]=0;
    if(verbosity) {
      std::cout<<goodtrigger[15-i]; 
    }
  }
  if(verbosity)std::cout<<"\n";
  //goodtrigger[10]=true;//In 2023, it was MBD N&S
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
      if(verbosity){
	std::cout<<"Initiating spin pattern from CAD\n";
      }
      for(int i = 0; i< 120; i++){
	SpinPatterns[BLUE][i]=bluepacket->iValue(i);
	SpinPatterns[YELLOW][i]=yellpacket->iValue(i);
	if(first && (true /*abort gap def test*/)){
	  first=false;
	  StartAbortGapPattern=i;
	}
      }
    }
    else{
      //******* placeholder until html delivery from C-AD is available ***********
      if(verbosity){
	std::cout<<"Initiating spin pattern from Dummy\n";
      }
      for(int i = 0; i< 120; i++){
	if (i % 2 == 0)SpinPatterns[BLUE][i]=1;
	else SpinPatterns[BLUE][i]=-1;
	if (int(0.5*i) % 2 == 0)SpinPatterns[YELLOW][i]=1;
	else SpinPatterns[YELLOW][i]=-1;
	if(i>110){
	  SpinPatterns[BLUE][i]=0;
	  SpinPatterns[YELLOW][i]=0;
	  if(first && (true /*abort gap def from HTML*/)){
	    first=false;
	    StartAbortGapPattern=i;
	  }
	}
      }
      // **************************************************************************
    }
  }
  Packet* pgl1 = e->getPacket(packetid_gl1);

  //Here we do the magic to identify the abort gap
  std::vector<int> gap[16];
  std::map<int, int> begingap;
  std::map<int, int> endgap;

  if(pgl1){
    int bunchnr = pgl1->lValue(0,"BunchNumber");
    for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
      //long long counts = pgl1->lValue(i,2); 
      //gl1_counter[i][bunchnr]=counts; 

      //With prdf pgl1->lValue(i,2); simply returns the current processed event number (which can shaddow the abort gap: the lagging bunch# at some point get back to the position of the others)
      //So instead, we increment the number of processed events per bunch number, for the various triggers
      gl1_counter[i][bunchnr]+=1;//(pgl1->lValue(i,2)>0)?1:0; 
    }
  }

  //Here we do the matching to identify the crossing shift
  if(evtcnt>EventCountThresholdGap){//Do we need to somehow wait or from the first events the scalers for all bunches would be already filled? Apparently at least 6,000 to start to see an abort gap with prdf...
    for(int i=0; i<16; i++){
      if(!goodtrigger[i]) continue;
      std::map<int, long long> tmpmap=gl1_counter[i];
      for(int emptyfill=0; emptyfill<9; emptyfill++){
	int myminimum=min_element(tmpmap.begin(),tmpmap.end(),[](const std::pair<int,long long>&lhs, const std::pair<int,long long>& rhs){return lhs.second<rhs.second;})->first;
	if(myminimum<111)gap[i].push_back(120+myminimum);
	else gap[i].push_back(myminimum);
	tmpmap.erase(myminimum);
      }
      begingap[i]=(*min_element(gap[i].begin(), gap[i].end()));
      endgap  [i]=(*max_element(gap[i].begin(), gap[i].end()));
      if(endgap[i]-begingap[i]>9) std::cout<<" Weird abort gap is larger than 9 bunches "<<endgap[i]<<" - "<<begingap[i]<<" for trigger "<<i<<std::endl;
    }
    for(auto ib=begingap.begin(); ib!=begingap.end(); ++ib){
      if(begingap.begin()->second!=ib->second) std::cout<<" Weird abort gap not in the same location between trigger bit 0 and trigger bit "<<ib->second<<std::endl;
    }
    StartAbortGapData=(begingap.begin()->second)%120;
  }
  else StartAbortGapData=ExpectedsPhenixGapPosition;//default value from config
  CrossingShift=StartAbortGapPattern-StartAbortGapData;
  if(verbosity)std::cout<<"Crossing shift: "<<CrossingShift<<" = ( "<<StartAbortGapPattern<<" - "<<StartAbortGapData<<" )\n";
  Packet* psmd = e->getPacket(packetid_smd);
  float smd_adc[32];
  if (psmd){

    int bunchnr = psmd->lValue(0,"BunchNumber");
    if(fake) bunchnr +=15+myRandomBunch->Integer(4);

    for(int ch=16; ch<47; ch++){//according to mapping in ZDC logbook entry #85 March 9th 2024 
      float signalFast = anaWaveformFast(psmd, ch);  // fast waveform fitting

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
  else{
    if(verbosity){
      std::cout<<"Did not retrieve all the information to enter the magic part of the code"<<std::endl;
      if(!psmd) std::cout<<"Missing ZDC/SMD packet "<<packetid_smd<<std::endl;
      if(!pgl1) std::cout<<"Missing GL1 packet "<<packetid_gl1<<std::endl;
    }
  }
  evtcnt++;
  return 0;

}

int LocalPolMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  return 0;
}
