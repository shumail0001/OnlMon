// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "TpcMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
#include <Event/msg_profile.h>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TTree.h>

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

TpcMon::TpcMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new TpcMon()

  serverid = 0; //default case - initializing in constructor
  //BCO initialization in TPCMon
  starting_BCO = -1;
  rollover_value = 0;
  current_BCOBIN = 0;

  return;
}

TpcMon::~TpcMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  return;
}

int TpcMon::Init()
{
  // read our calibrations from TpcMonData.dat
  const char *tpccalib = getenv("TPCCALIB");
  if (!tpccalib)
  {
    std::cout << "TPCCALIB environment variable not set" << std::endl;
    exit(1);
  }
  std::string fullfile = std::string(tpccalib) + "/" + "TpcMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  tpchist1 = new TH1F("tpcmon_hist1", "test 1d histo", 101, 0., 100.);
  tpchist2 = new TH2F("tpcmon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);

  //TPC GEM Module Displays
  NorthSideADC = new TH2F("NorthSideADC" , "ADC Counts North Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  SouthSideADC = new TH2F("SouthSideADC" , "ADC Counts South Side", N_thBins, -TMath::Pi()/12. , 23.*TMath::Pi()/12. , N_rBins , rBin_edges );
  //

  // ADC vs Sample
  ADC_vs_SAMPLE = new TH2F("ADC_vs_SAMPLE", "ADC Counts vs Sample: WHOLE SECTOR", 360, 0, 360, 256, 0, 1024);
  ADC_vs_SAMPLE -> SetXTitle("Sector XX: ADC Time bin [1/20MHz]");
  ADC_vs_SAMPLE -> SetYTitle("ADC [ADU]");

  // Sample size distribution 1D histogram
  sample_size_hist = new TH1F("sample_size_hist" , "Distribution of Sample Sizes in Events", 1000, 0.5, 1000.5);
  sample_size_hist->SetXTitle("sample size");
  sample_size_hist->SetYTitle("counts");

  // entries vs FEE*8 + SAMPA Number
  Check_Sums = new TH1F("Check_Sums" , "Entries vs Fee*8 + SAMPA in Events",208,-0.5, 207.5);
  Check_Sums->SetXTitle("FEE_NUM*8 + SAMPA_ADRR");
  Check_Sums->SetYTitle("Entries");
  Check_Sums->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it

  // checksum error vs FEE*8 + SAMPA Number
  Check_Sum_Error = new TH1F("Check_Sum_Error" , "Check Sum Error Probability vs Fee*8 + SAMPA in Events",208,-0.5, 207.5);
  Check_Sum_Error->SetXTitle("FEE_NUM*8 + SAMPA_ADDR");
  Check_Sum_Error->SetYTitle("Prob. Check. Sum. Err.");
  Check_Sum_Error->Sumw2(kFALSE); //explicity turn off Sumw2 - we do not want it

  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, tpchist1);  // uses the TH1->GetName() as key
  se->registerHisto(this, tpchist2);
  se->registerHisto(this, NorthSideADC);
  se->registerHisto(this, SouthSideADC);
  se->registerHisto(this, sample_size_hist);
  se->registerHisto(this, Check_Sum_Error);
  se->registerHisto(this, Check_Sums);
  se->registerHisto(this, ADC_vs_SAMPLE);
  Reset();
  return 0;
}

int TpcMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it

  // we reset the BCO for the new run
  starting_BCO = -1;
  rollover_value = 0;
  current_BCOBIN = 0;

  return 0;
}

int TpcMon::process_event(Event *evt/* evt */)
{

  //std::cout << "TpcMon::process_event(Event * evt) Processing Event" << std::endl;

  if (evt == nullptr)
  {
    std::cout << "TpcMon::process_event - Event not found" << std::endl;
    return -1;
  }

  if (evt->getEvtType() >= 8)  /// special events
  {
    std::cout << "TpcMon::process_event - Special Event type >= 8, moving on" << std::endl;
    return -1;
  }

  //reset these each event
  float North_Side_Arr[36] = {0};
  float South_Side_Arr[36] = {0};

  for( int packet = 4000; packet < 4231; packet++) //packet 4000 or 4001 = Sec 00, packet 4230 or 4231 = Sec 23
  {
    Packet* p = evt->getPacket(packet);
    if (!p)
    {
      //std::cout << "TpcMon::process_event - No packet numbered " << packet << " in this event!!" << std::endl;
      continue;
    }
    else
    {
      //std::cout << "____________________________________" << std::endl;
      //std::cout << "Packet # " << packet << std::endl;
      int nr_of_waveforms = p->iValue(0, "NR_WF");
      //std::cout << "Hello Waveforms ! - There are " << nr_of_waveforms << " of you !" << std::endl;
      for( int wf = 0; wf < nr_of_waveforms; wf++)
      {

        int current_BCO = p->iValue(wf, "BCO") + rollover_value;
        if (starting_BCO < 0)
        {
          starting_BCO = current_BCO;
        }

        if (current_BCO < starting_BCO)  // we have a rollover
        {
          rollover_value += 0x100000;
          current_BCO = p->iValue(wf, "BCO") + rollover_value;
          starting_BCO = current_BCO;
          current_BCOBIN++;
        }


        int fee = p->iValue(wf, "FEE");
        int sampaAddress = p->iValue(wf, "SAMPAADDRESS");
        int checksumError = p->iValue(wf, "CHECKSUMERROR");
        //int channel = p->iValue(wf, "CHANNEL");

        Check_Sums->Fill(fee*8 + sampaAddress); 
        if( checksumError == 1){Check_Sum_Error->Fill(fee*8 + sampaAddress);}

        int nr_Samples = p->iValue(wf, "SAMPLES");
        sample_size_hist->Fill(nr_Samples);

        serverid = MonitorServerId();
        //std::cout<<"Sector = "<< serverid <<" FEE = "<<fee<<" channel = "<<channel<<std::endl;

        for( int s =0; s < nr_Samples ; s++ ){

          //int t = s + 2 * (current_BCO - starting_BCO);

          //std::cout<<"Sector = "<< serverid <<" FEE = "<<fee<<" channel = "<<channel<<", sample = "<<s<<""<<std::endl;
          int adc = p->iValue(wf,s);

          if( checksumError == 0){ADC_vs_SAMPLE -> Fill(s, adc);}

          //increment 
          if(serverid >= 0 && serverid < 12 ){ North_Side_Arr[ Index_from_Module(serverid,fee) ] += adc;}
          else {South_Side_Arr[ Index_from_Module(serverid,fee) ] += adc;}

        }
      }
      delete p;
    }
  }



  evtcnt++;
  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("tpchist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram
  tpchist1->Fill((float) idummy);
  tpchist2->Fill((float) idummy, (float) idummy, 1.);

  //fill the TPC module displays
  float r, theta;

  //dummy data
  //float North_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };
  //float South_Side_Arr[36] = { 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50, 12, 8, 40, 39, 80, 50 };

  for(int tpciter = 1; tpciter < 73 ; tpciter++){

    Locate(tpciter, &r, &theta);
    //std::cout << "r is: "<< r <<" theta is: "<< theta <<"\n";

    if(tpciter < 37){ //South side
      NorthSideADC->Fill(theta,r, North_Side_Arr[tpciter-1]); //fill South side with the weight = bin content
    }
    else { //North side
      SouthSideADC->Fill(theta,r,South_Side_Arr[tpciter-37]); //fill North side with the weight = bin content
    }
 
  }
  //


  return 0;
}

int TpcMon::Index_from_Module(int sec_id, int fee_id)
{
  int mod_id;
 
  if( fee_id == 2 || fee_id == 3 || fee_id == 4 || fee_id == 13 || fee_id == 16 || fee_id == 17 ){mod_id = 3*sec_id + 0;} // R1 

  else if( fee_id == 0 || fee_id == 1 || fee_id == 11 || fee_id == 12 || fee_id == 14 || fee_id == 15 || fee_id == 18 || fee_id == 19 ){mod_id = 3*sec_id + 1;} // R2

  else if( fee_id == 5 || fee_id == 6 || fee_id ==7 || fee_id == 8 || fee_id == 9 || fee_id == 10 || fee_id == 20 || fee_id == 21 || fee_id == 22 || fee_id == 23 || fee_id == 24 || fee_id == 25 ){mod_id = 3*sec_id + 2;} // R3

  else mod_id = 0;

  return mod_id;
}

void TpcMon::Locate(int id, float *rbin, float *thbin)
{
   float CSIDE_angle_bins[12] = { 0.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 6.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 }; //CCW from x = 0 (RHS horizontal)

   float ASIDE_angle_bins[12] = { 6.1*2.*TMath::Pi()/12 , 5.1*2.*TMath::Pi()/12 , 4.1*2.*TMath::Pi()/12 , 3.1*2.*TMath::Pi()/12 , 2.1*2.*TMath::Pi()/12 , 1.1*2.*TMath::Pi()/12 , 0.1*2.*TMath::Pi()/12 , 11.1*2.*TMath::Pi()/12 , 10.1*2.*TMath::Pi()/12 , 9.1*2.*TMath::Pi()/12 , 8.1*2.*TMath::Pi()/12 , 7.1*2.*TMath::Pi()/12  }; //CCW from x = 0 (RHS horizontal)

   int modid3 = id % 3;

   switch(modid3) {
     case 1:
       *rbin = 0.4; //R1
       break;
     case 2:
       *rbin = 0.6; //R2
       break;
     case 0:
       *rbin = 0.8; //R3
       break;
   }

  if( id < 37){
    *thbin = CSIDE_angle_bins[TMath::FloorNint((id-1)/3)];
  }
  else if( id >= 37){
    *thbin = ASIDE_angle_bins[TMath::FloorNint((id-37)/3)];
  }
}


int TpcMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

