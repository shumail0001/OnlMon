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
#include <Event/eventReceiverClient.h>

#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TRandom.h>
#include <TString.h>

#include <cmath>
#include <ctime>
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
  iPoint=0;
  for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
    goodtrigger[i]=false; 
  }
  goodtrigger[10]=true; 
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
    msg_config<<lpconfigfilename<<" could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL,msg_config.str(),2);
    exit(1);
  }
  int err_counter=0;
  TString key;
  TString  val;
  while(!config.eof()){
    config>>key>>val;
    std::cout<<"Config in "<<config.good()<<" state\n";
    if(!config.good())break;
    key.ToLower();
    std::cout<<"Reading "<<key<<" with value "<<val<<std::endl;
    if(key=="trigger"){
      if(!val.IsDigit()){
	std::cout<<key<<": expecting an integer (2^(trigger bit number))\n keep only bit 10 as default value.\n";
	goodtrigger[10]=true;
      }
      else {
	int ival=val.Atoi();
	if(ival<0||ival>65535){
	  std::cout<<key<<": value outside expected range [0; 65535]\n Keep only bit 10 as default value.\n";
	  goodtrigger[10]=true;
	}
	else{
	  for(int i=0; i<16; i++){
	    goodtrigger[i]=false;
	    if((ival&(1<<i))){
	      goodtrigger[i]=true;
	    }
	  }
	}
      }
    }
    else if(key=="testfake"){
      if(!val.IsDigit()){
	std::cout<<key<<": expecting 0/1 for true or false\n Keep false as default.\n";
	fake=false;
	erc = new eventReceiverClient("gl1daq");
      }
      else{
	int ival=val.Atoi();
	if(ival!=0 && ival!=1){
	  std::cout<<key<<": expecting 0/1 for true or false\n Keep false as default.\n";
	  fake=false;
	  erc = new eventReceiverClient("gl1daq");
	}
	else{
	  if(ival==1){
	    fake=true;
	    erc = nullptr;
	  }
	  else fake=false;
	  erc = new eventReceiverClient("gl1daq");
	}
      }
    }
    else if(key=="sphenixgap"){
      if(!val.IsDigit()){
	std::cout<<key<<": expecting an integer as the first bunch number of the continuous gap sequence\n Keep 117 as default.\n";
	ExpectedsPhenixGapPosition=117;
      }
      else{
	int ival=val.Atoi();
	if(ival>=0 && ival<120){
	  ExpectedsPhenixGapPosition=ival; 
	}
	else{
	  std::cout<<key<<": value outside expected range [0;119]\n Keep 117 as default value\n";
	  ExpectedsPhenixGapPosition=117; 
	}
      }
    }
    else if(key=="threshold"){
      if(!val.IsDigit()){
	std::cout<<key<<": expecting a positive integer\n Keep 6000 as default value\n";
	EventCountThresholdGap=6000;
      }
      else{
	int ival=val.Atoi();
	if(ival>0&&ival<1e5)EventCountThresholdGap=ival;
	else{
	  std::cout<<key<<": integer seems too big (>1e5)\n Keep default 6000\n";
	  EventCountThresholdGap=6000;
	}
      }
    }
    else if(key=="verbosity"){
      if(!val.IsDigit()){
	std::cout<<key<<": expecting 0/1 for false or true\n Keep 0(false) as default\n";
	verbosity=false;
      }
      else{
	int ival=val.Atoi();
	if(ival!=0 && ival!=1){
	  std::cout<<key<<": value should be 0 or 1 for false of true\n Keep 0(false) as default\n";
	  verbosity=false;
	}
	else if(ival==1) {
	  std::cout<<"Making it verbose"<<std::endl;
	  verbosity=true;
	}
      }
    }
    else if(key=="x0north"){
      if(val.IsFloat()){
	if(val.Atof()<-3. || val.Atof()>3.){
	  std::cout<<key<<": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value."<<std::endl;
	}
	else{
	  ZeroPosition[1]=val.Atof();
	}
      }
      else{
	std::cout<<key<<": value is not a float\n keeping 0.0 as default value"<<std::endl;
	ZeroPosition[1]=0.;
      }
    }
    else if(key=="y0north"){
      if(val.IsFloat()){
	if(val.Atof()<-3. || val.Atof()>3.){
	  std::cout<<key<<": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value."<<std::endl;
	}
	else{
	  ZeroPosition[0]=val.Atof();
	}
      }
      else{
	std::cout<<key<<": value is not a float\n keeping 0.0 as default value"<<std::endl;
	ZeroPosition[0]=0.;
      }
      
    }
    else if(key=="x0south"){
      if(val.IsFloat()){
	if(val.Atof()<-3. || val.Atof()>3.){
	  std::cout<<key<<": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value."<<std::endl;
	}
	else{
	  ZeroPosition[3]=val.Atof();
	}
      }
      else{
	std::cout<<key<<": value is not a float\n keeping 0.0 as default value"<<std::endl;
	ZeroPosition[3]=0.;
      }      
    }
    else if(key=="y0south"){
      if(val.IsFloat()){
	if(val.Atof()<-3. || val.Atof()>3.){
	  std::cout<<key<<": value outside the expected range [-3.;3]cm\n keeping 0.0 as default value."<<std::endl;
	}
	else{
	  ZeroPosition[2]=val.Atof();
	}
      }
      else{
	std::cout<<key<<": value is not a float\n keeping 0.0 as default value"<<std::endl;
	ZeroPosition[2]=0.;
      }      
    }
    else if(key=="thresholdasymnewpoint"){
      if(!val.IsDigit()){
	std::cout<<key<<": value is expected to be a positive integer\n Keeping default value of 5000 events."<<std::endl;
      }
      else{
	std::cout<<key<<"Changed from "<<EventsAsymmetryNewPoint<<" to "<<val.Atoi()<<std::endl;
	EventsAsymmetryNewPoint=val.Atoi();
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

  std::string lpmappingfilename=std::string(locpolcal)+"/"+"ChannelMapping.txt";
  std::ifstream mapping(lpmappingfilename);
  std::ostringstream msg_mapping(lpmappingfilename);

  if(!mapping){
    msg_mapping<<lpmappingfilename<<" could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL,msg_mapping.str(),2);
    exit(1);
  }
  int adc, array;
  std::string ChannelName;
  for(int i=0; i<128; i++){
    mapping>>adc>>array>>ChannelName;
    Chmapping[adc]=array;
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
    msg<<calibfilename<<" could not be opened.";
    se->send_message(this, MSG_SOURCE_LOCALPOL, MSG_SEV_FATAL,msg.str(),2);
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

  h_time = new TProfile("h_times","h_time",5000,-0.5,4999.5);
  se->registerHisto(this, h_time);


  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  h_Asym         = new TH1D***[2];
  h_AsymScramble = new TH1D***[2];
  for(int beam=0; beam<2; beam++){
    h_Asym[beam]         = new TH1D**[2];
    h_AsymScramble[beam] = new TH1D**[2];
    for(int method=0; method<2; method++){
      h_Asym[beam][method]        =new TH1D*[2];
      h_AsymScramble[beam][method]=new TH1D*[2];
      for(int orient=0; orient<2; orient++){
	h_Asym[beam][method][orient]         = new TH1D(Form("h_Asym%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),Form("Fwd %s %s %s Asym.",BeamName[beam].Data(),Orientation[orient].Data(),MethodName[method].Data()),5000,-0.5,4999.5);
	h_AsymScramble[beam][method][orient] = new TH1D(Form("h_AsymScramble%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),Form("Bwk %s %s %s Asym.",BeamName[beam].Data(),Orientation[orient].Data(),MethodName[method].Data()),5000,-0.5,4999.5);

	se->registerHisto(this,h_Asym[beam][method][orient]);
	se->registerHisto(this,h_AsymScramble[beam][method][orient]);
      }
    }
  }

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
    stored_gl1p_files[e->getEvtSequence()]=bunchnr;
    for (int i = 0; i < 16; i++ ) { //16 triggers for gl1p 
      //long long counts = pgl1->lValue(i,2); 
      //gl1_counter[i][bunchnr]=counts; 

      //With prdf pgl1->lValue(i,2); simply returns the current processed event number (which can shaddow the abort gap: the lagging bunch# at some point get back to the position of the others)
      //So instead, we increment the number of processed events per bunch number, for the various triggers
      gl1_counter[i][bunchnr]+=1;//(pgl1->lValue(i,2)>0)?1:0; 
    }
  }



  /******Here we do the matching to identify the crossing shift   ******************/
  if(evtcnt>EventCountThresholdGap){//Do we need to somehow wait or from the first events the scalers for all bunches would be already filled? Apparently at least 6,000 to start to see an abort gap with prdf...
    for(int i=0; i<16; i++){
      if(!goodtrigger[i]) continue;
      std::map<int, long long> tmpmap=gl1_counter[i];
      //for(std::map<int,long long>::iterator ittest=tmpmap.begin(); ittest!=tmpmap.end(); ++ittest){
      //	std::cout<<ittest->second<<" ";
      //}
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
  /************* end of crossing shift *********************/



  Packet* psmd = e->getPacket(packetid_smd);
  float smd_adc[32];
  if (psmd){

    int bunchnr=0;// = psmd->lValue(0,"BunchNumber");
    Event* egl1=nullptr;
    if(erc) {
      egl1=erc->getEvent(e->getEvtSequence());
    }
    if(egl1){
      Packet* ptmpgl1=egl1->getPacket(packetid_gl1);
      if(ptmpgl1){
	bunchnr=ptmpgl1->lValue(0,"BunchNumber");
	delete ptmpgl1;
	ptmpgl1=nullptr;
      }
      else{
	if(verbosity){
	  std::cout<<"Failed grabing gl1 from event receiver, Bunch number unknown"<<std::endl;
	}
      }
      delete egl1;
      egl1=nullptr;
    }
    //else{
    //  std::map<int,int>::iterator it;
    //  it=stored_gl1p_files.find(e->getEvtSequence());
    //  if(it!=stored_gl1p_files.end()) {
    //	bunchnr=it->second;
    //	std::cout<<bunchnr<<std::endl;
    //  }
    //  else {
    //	bunchnr=0;
    //	std::cout<<"Trick also failed to retrieve bunch number"<<std::endl;
    //  }
    //}
    
    if(fake) bunchnr +=15+myRandomBunch->Integer(4);

    //get minimum on ZDC second module
    signalZDCN2=anaWaveformFast(psmd, ZDCN2); 
    signalZDCS2=anaWaveformFast(psmd, ZDCS2); 


    //for(int ch=16; ch<47; ch++){//according to mapping in ZDC logbook entry #85 March 9th 2024 
    for(std::map<int,int>::iterator it=Chmapping.begin(); it!=Chmapping.end(); ++it){//new mapping for ZDC/SMD/Veto with 2 ADC boards//May 13th 2024
      if(it->second<0) continue;
      if(it->second>47) continue;
      float signalFast = anaWaveformFast(psmd, it->first);  // fast waveform fitting

      int ch=it->second;
      //Scale according to relative gain calibration factor
      if(ch<32) smd_adc[ch-16]=signalFast*smd_north_rgain[ch-16];
      else smd_adc[ch-16]=signalFast*smd_south_rgain[ch-32];
    }
    if(signalZDCN2<65 && signalZDCS2<65) return 0;
    float Weights[4]={0};
    memset(Weights, 0, sizeof(Weights));
    float AveragePosition[4]={0};
    memset(AveragePosition,0,sizeof(AveragePosition));
    
    for(int ch=0; ch<8; ch++){
      Weights[0]         += smd_adc[ch];
      AveragePosition[0] += ConversionSign[0]*pitchY*(ch-nchannelsY/2)*smd_adc[ch];//North Y direction (Asym in blue)
      Weights[2]         += smd_adc[ch+16];
      AveragePosition[2] += ConversionSign[2]*pitchY*(ch-nchannelsY/2)*smd_adc[ch+16];//South Y direction (Asym in Yellow)

      if(ch==7) continue;
      Weights[1]         += smd_adc[ch+8];
      AveragePosition[1] += ConversionSign[1]*pitchX*(ch-nchannelsX/2)*smd_adc[ch+8];//North X direction (Asym in Blue)
      Weights[3]         += smd_adc[ch+24];
      AveragePosition[3] += ConversionSign[3]*pitchX*(ch-nchannelsX/2)*smd_adc[ch+24];//South X direction (Asym in Yellow)
    }
    
    for(int i=0; i<4; i++){
      if(Weights[i]>0.0) AveragePosition[i]/=Weights[i];
      //else AveragePosition[i]=0.;
      else continue;//most likely the most appropriate

      if(AveragePosition[i]<ZeroPosition[i]-0.5){
	// (i/2)=0 for blue beam, =1 for yellow beam
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i]->Fill(1);//Left for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i]->Fill(3);//Right for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i]->Fill(1);//Left for pointing down
      }
      else if(AveragePosition[i]>ZeroPosition[i]+0.5){
	// (i/2)=0 for blue beam, =1 for yellow beam   
	if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)>0) h_Counts[i]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2].at((120+bunchnr+CrossingShift)%120)<0) h_Counts[i]->Fill(2);//Right for pointing down

	//we swap the spin pattern to get random orientation and check biased asymmetry
	if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)>0) h_CountsScramble[i]->Fill(0);//Left for pointing up
	else if(SpinPatterns[i/2==0?1:0].at((120+bunchnr+CrossingShift)%120)<0) h_CountsScramble[i]->Fill(2);//Right for pointing down
      }
    }
    h_time->Fill(iPoint,e->getTime());
    evtcnt++;
  }
  else{
    if(verbosity){
      std::cout<<"Missing ZDC/SMD packet "<<packetid_smd<<std::endl;
    }
  }

  /**** Compute asymmetries if we have enough events ****************/
  if(evtcnt%EventsAsymmetryNewPoint==0){
    
    for(int ibeam=0; ibeam<2; ibeam++){
      for(int orient=0; orient<2; orient++){
	
	double L_U = h_Counts[2*ibeam+orient]->GetBinContent(1);
	double R_D = h_Counts[2*ibeam+orient]->GetBinContent(2);
	double L_D = h_Counts[2*ibeam+orient]->GetBinContent(3);
	double R_U = h_Counts[2*ibeam+orient]->GetBinContent(4);
	//if(L_U<0){
	//  std::cout<<iPoint<<" "<<evtcnt<<" "<<h_Counts[2*ibeam+orient]->GetEntries()<<" "<<std::endl;
	//}
	double* asymresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);
	
	h_Asym[ibeam][0][orient]->SetBinContent(iPoint+1,asymresult[0]);
	h_Asym[ibeam][0][orient]->SetBinError(iPoint+1,asymresult[1]);
	
	h_Asym[ibeam][1][orient]->SetBinContent(iPoint+1,asymresult[2]);
	h_Asym[ibeam][1][orient]->SetBinError(iPoint+1,asymresult[3]);

	L_U = h_CountsScramble[2*ibeam+orient]->GetBinContent(1);
	R_D = h_CountsScramble[2*ibeam+orient]->GetBinContent(2);
	L_D = h_CountsScramble[2*ibeam+orient]->GetBinContent(3);
	R_U = h_CountsScramble[2*ibeam+orient]->GetBinContent(4);
      
	double* scrambleresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);
	
	h_AsymScramble[ibeam][0][orient]->SetBinContent(iPoint+1,scrambleresult[0]);
	h_AsymScramble[ibeam][0][orient]->SetBinError(iPoint+1,scrambleresult[1]);
	
	h_AsymScramble[ibeam][1][orient]->SetBinContent(iPoint+1,scrambleresult[2]);
	h_AsymScramble[ibeam][1][orient]->SetBinError(iPoint+1,scrambleresult[3]);

	delete asymresult;
	delete scrambleresult;
      }
    }
    //Now that everything has been calculated, let move to the next point for next event
    iPoint++;
  }

  return 0;

}

int LocalPolMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  iPoint = 0;
  for(int i=0; i<4; i++){
    h_Counts[i]->Reset();
    h_CountsScramble[i]->Reset();
  }
  h_time->Reset();
  return 0;
}

double* LocalPolMon::ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U){
  double* result = new double[4];
      double leftA  = L_U+R_D;
      double rightA = L_D+R_U;
      double tmpNumA = leftA-rightA;
      double tmpDenA = leftA+rightA;
      result[0] = 0;
      result[1] = 0;
	
      if(tmpDenA>0){
	result[0] = tmpNumA/tmpDenA;
	result[1] = 2*sqrt(pow(rightA,2)*leftA+pow(leftA,2)*rightA)/pow(tmpDenA,2);      
      }
	
      double leftG = sqrt(L_U*R_D);
      double rightG= sqrt(L_D*R_U);
      double tmpNumG = leftG-rightG;
      double tmpDenG = leftG+rightG;
      result[2] = 0;
      result[3] = 0;
	
      if(tmpDenG>0){
	result[2] = tmpNumG/tmpDenG;
	result[3] = sqrt(pow(rightG,2)*leftA+pow(leftG,2)*rightA)/pow(tmpDenG,2);
      }
      return result;
}
