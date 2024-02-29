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

#include <gsl/gsl_const.h>
#include <gsl/gsl_math.h>

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
  const float MAX_ENERGY1 = 15000.;
  const float MAX_ENERGY2 = 15000.;
  const int BIN_NUMBER = 1500;

    
  //  gRandom->SetSeed(rand());
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

  //getting gains
  float col1, col2, col3;
  std::string gainfile = std::string(zdccalib) + "/" + "/ZdcCalib.pmtgain";
  std::ifstream gain_infile(gainfile);
  std::ifstream msg(gainfile);
  
  // if (!gain_infile)
  // {
  //   msg << gainfile << " could not be opened." ;
  //   OnlMonServer *se = OnlMonServer::instance();
  //   se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
  //   exit(1);
  // }

  for (int i = 0; i < 32; i++)
  {
    gain_infile >> col1 >> col2 >> col3;
    gain[i] = col1;
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
  {
    smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
  }

  for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
  {
    smd_north_rgain[i] = gain[i + 16];  // 0-7: y channels, 8-14: x channels, 15: analog sum
  }
  
  gain_infile.close();


  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");


  // Create hitograms
  zdc_adc_north = new TH1F("zdc_adc_north", "ZDC ADC north", BIN_NUMBER, 0, MAX_ENERGY1);
  zdc_adc_south = new TH1F("zdc_adc_south", "ZDC ADC south", BIN_NUMBER, 0, MAX_ENERGY2);
  
  // smd
  // north smd
  smd_hor_north = new TH1F("smd_hor_north", "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_ver_north = new TH1F("smd_ver_north", "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  smd_sum_hor_north = new TH1F ("smd_sum_hor_north", "SMD North y", 512, 0, 2048);
  smd_sum_ver_north = new TH1F ("smd_sum_ver_north", "SMD North x", 512, 0, 2048);
  smd_hor_north_small = new TH1F ("smd_hor_north_small", "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_ver_north_small = new TH1F ("smd_ver_north_small", "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  smd_hor_north_good = new TH1F ("smd_hor_north_good", "Beam centroid distribution, SMD North y", 296, -5.92, 5.92);
  smd_ver_north_good = new TH1F ("smd_ver_north_good", "Beam centroid distribution, SMD North x", 220, -5.5, 5.5);
  // south smd 
  smd_hor_south = new TH1F("smd_hor_south", "Beam centroid distribution, SMD South y", 296, -5.92, 5.92);
  smd_ver_south = new TH1F("smd_ver_south", "Beam centroid distribution, SMD South x", 220, -5.5, 5.5);
  smd_sum_hor_south = new TH1F ("smd_sum_hor_south", "SMD South y", 640, 0, 2560);
  smd_sum_ver_south = new TH1F ("smd_sum_ver_south", "SMD South x", 640, 0, 2560);
  // smd values
  smd_value = new TH2F("smd_value", "SMD channel# vs value", 1024, 0, 4096, 32, 0, 32);
  smd_value_good = new TH2F("smd_value_good", "SMD channel# vs value", 1024, 0, 4096, 16, 0, 16);
  smd_value_small = new TH2F("smd_value_small", "SMD channel# vs value", 1024, 0, 4096, 16, 0, 16);

  OnlMonServer *se = OnlMonServer::instance();
  //register histograms with server otherwise client won't get them
  se->registerHisto(this, zdc_adc_north );
  se->registerHisto(this, zdc_adc_south );
  // smd
  // north smd
  se->registerHisto(this, smd_hor_north);
  se->registerHisto(this, smd_ver_north);
  se->registerHisto(this, smd_sum_hor_north);
  se->registerHisto(this, smd_sum_ver_north);
  se->registerHisto(this, smd_hor_north_small);
  se->registerHisto(this, smd_ver_north_small);
  se->registerHisto(this, smd_hor_north_good);
  se->registerHisto(this, smd_ver_north_good);
  // south smd
  se->registerHisto(this, smd_hor_south);
  se->registerHisto(this, smd_ver_south);
  se->registerHisto(this, smd_sum_hor_south);
  se->registerHisto(this, smd_sum_ver_south);
  // smd values
  se->registerHisto(this, smd_value);
  se->registerHisto(this, smd_value_good);
  se->registerHisto(this, smd_value_small);

    
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

  float totalzdcsouthsignal = 0.;
  float totalzdcnorthsignal = 0.;
  int packet = 12001;

  Packet *p = e->getPacket(packet);
  if (p)
  {
  
    // zdc_adc
    for (int c = 0; c < p->iValue(0, "CHANNELS"); c++)
    {
      std::vector<float> resultFast = anaWaveformFast(p, c);  // fast waveform fitting
      float signalFast = resultFast.at(0);
      float signal = signalFast;
        
      unsigned int towerkey = TowerInfoDefs::decode_zdc(c);
      int zdc_side = TowerInfoDefs::get_zdc_side(towerkey);
        
      int mod = c%2;

      // put code to assign smd_adc[i]

      if (c < 16) {zdc_adc[c] = signal;}
      else {smd_adc[c - 16] = signal;}
      


      if (mod != 0) continue;
      
      if((c < 16) && ((c != 6) && (c != 14)))
      {
        if (zdc_side == 0)
        {
          totalzdcsouthsignal+= signal;
        }
        else if (zdc_side == 1)
        {
          totalzdcnorthsignal+= signal;
        }
        else
        {
          std::cout << "arm bin not assigned ... " << std::endl;
          return -1;
        }
      }  //select zdc high gain channels only

    



    }  // channel loop end

    // call the functions
    CompSmdAdc();
    CompSmdPos();
    CompSumSmd();
    

    // BOOLEANS, INTs AND OTHER DEFINITIONS

    

    int s_ver = 0;
    int s_hor = 0;

    int n_ver  = 0;
    int n_hor  = 0;

    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i] > 8 ) {n_hor ++;}
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 8] > 5 ) {n_ver ++;}
    }

    for ( int i = 0; i < 8; i++)
    {
      if ( smd_adc[i + 16] > 8 ) {s_hor++;}
    }
    for ( int i = 0; i < 7; i++)
    {
      if ( smd_adc[i + 24] > 5 ) {s_ver++;}
    }

    bool fired_smd_hor_n = (n_hor  > 1);
    bool fired_smd_ver_n = (n_ver  > 1);

    bool fired_smd_hor_s = (s_hor > 1);
    bool fired_smd_ver_s = (s_ver > 1);

    // bool fill_hor_south = false;
    // bool fill_ver_south = false;

    // bool fill_hor_north = false;
    // bool fill_ver_north = false;


    //compute, if smd is overloaded
    bool smd_ovld_north = false;
    bool smd_ovld_south = false;
    //smd_ovld_south = (zdc_adc[0] > 1000);
    //smd_ovld_north = (zdc_adc[4] > 1000);

    bool ped_smd_hnorth = true; //in 200GeV, it was: (smd_sum[2] > 800.);
    bool ped_smd_vnorth = true; //in 200GeV, it was: (smd_sum[3] > 700.);


    // FILLING OUT THE HISTOGRAMS

    // PHENIX had: if ( ped_zdc_south && !did_laser_fire && fired_smd_hor_s && fired_smd_ver_s && ovfbool[0] && ovfbool[4] && !smd_ovld_south)
    if (fired_smd_hor_s && fired_smd_ver_s && !smd_ovld_south)
    {
      // fill_hor_south = true;
      // fill_ver_south = true;
      smd_hor_south->Fill( smd_pos[0] );
      smd_ver_south->Fill( smd_pos[1] );
      for (int i = 0 ; i < 8; i++)
      {
        smd_value->Fill(smd_adc[i], float(i) );
        smd_value->Fill(smd_adc[i + 8], float(i) + 8. );
      }
      // if ((zdc_adc[0] > 200.))
      // {
        for (int i = 0; i < 8; i++)
        {
          smd_value_good->Fill(smd_adc[i], float(i));
          smd_value_good->Fill(smd_adc[i + 8], float(i) + 8);
        }
      // }
      // if ((zdc_adc[0] <= 200.))
      // {
        for (int i = 0; i < 8; i++)
        {
          smd_value_small->Fill(smd_adc[i], float(i));
          smd_value_small->Fill(smd_adc[i + 8], float(i) + 8);
        }
      // }
    }

    // PHENIX had: if (ped_zdc_north && ped_smd_hnorth && ovfbool[0] && ovfbool[4] && !smd_ovld_north &&  fired_smd_hor_n && !did_laser_fire)
    if ( fired_smd_hor_n && ped_smd_hnorth && !smd_ovld_north)
    {
      // fill_hor_north = true;
      smd_hor_north->Fill( smd_pos[2] );
      // zdc_hor_north->Fill( zdc_adc[4] / ADC_to_GeV_north, smd_pos[2] );
      for (int i = 0; i < 8; i++)
      {
        smd_value->Fill(smd_adc[i + 16], float(i) + 16);
      }
      if ((zdc_adc[4] > 200.))
      {
        smd_hor_north_good->Fill( smd_pos[2] );
        for (int i = 0; i < 8; i++)
        {
          smd_value_good->Fill(smd_adc[i + 16], float(i) + 16.);
        }
      }
      if ((zdc_adc[4] <= 200.))
      {
        smd_hor_north_small->Fill( smd_pos[2] );
        for (int i = 0; i < 8; i++)
        {
          smd_value_small->Fill(smd_adc[i + 16], float(i) + 16.);
        }
      }
    }

    // PHENIX had: if (ped_zdc_north && ped_smd_vnorth && ovfbool[0] && ovfbool[4] && !smd_ovld_north && fired_smd_ver && !did_laser_fire)
    if (fired_smd_ver_n && ped_smd_vnorth && !smd_ovld_north)
    {
      // fill_ver_north = true;
      smd_ver_north->Fill( smd_pos[3] );
      // zdc_ver_north->Fill( zdc_adc[4] / ADC_to_GeV_north, smd_pos[3] );
      for (int i = 0; i < 8; i++)
      {
        smd_value->Fill(smd_adc[i + 24], float(i) + 24);
      }
      if ((zdc_adc[4] > 200.))
      {
        smd_ver_north_good->Fill( smd_pos[3] );
        for (int i = 0; i < 8; i++)
        {
          smd_value_good->Fill(smd_adc[i + 24], float(i) + 24.);
        }
      }
      if ((zdc_adc[4] <= 200.))
      {
        smd_ver_north_small->Fill( smd_pos[3] );
        for (int i = 0; i < 8; i++)
        {
          smd_value_small->Fill(smd_adc[i + 24], float(i) + 24.);
        }
      }
    }




  }    // if packet good

  zdc_adc_south->Fill(totalzdcsouthsignal);
  zdc_adc_north->Fill(totalzdcnorthsignal);
  
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


// void ZdcMon::GetCalConst()
// {
//   ostringstream pedfile, gainfile, ovf0file, ovf1file, calibdir;
//   //getting directory where the calibration files are
//   calibdir.str("");
//   if (getenv("ZDCCALIBDIR"))
//   {
//     calibdir << getenv("ZDCCALIBDIR");
//   }
//   else
//   {
//     calibdir << getenv("ONLMON_MAIN") << "/share";
//   }
//   //getting gains
//   gainfile.str("");
//   gainfile << calibdir.str().c_str() << "/ZdcCalib.pmtgain";
//   ifstream gain_infile(gainfile.str().c_str(), ios::in);
//   if (!gain_infile)
//   {
//     ostringstream msg;
//     msg << gainfile << " could not be opened." ;
//     OnlMonServer *se = OnlMonServer::instance();
//     se->send_message(this, MSG_SOURCE_ZDC, MSG_SEV_FATAL, msg.str(), 2);
//     exit(1);
//   }

//   for (int i = 0; i < 32; i++)
//   {
//     gain_infile >> col1 >> col2 >> col3;
//     gain[i] = col1;
//   }

//   for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
//   {
//     smd_south_rgain[i] = gain[i];  // 0-7: y channels, 8-14: x channels, 15: analog sum
//   }

//   for (int i = 0; i < 16; i++)  // relative gains of SMD north channels
//   {
//     smd_north_rgain[i] = gain[i + 16];  // 0-7: y channels, 8-14: x channels, 15: analog sum
//   }

// }

void ZdcMon::CompSmdAdc() // mulitplying by relative gains
{
  for (int i = 0; i < 15; i++) // last one is reserved for analogue sum
  {
    // multiply SMD channels with their gain factors
    // to get the absolute ADC values in the same units
    //rgains come from CompSmdAdc()
    smd_adc[i] = smd_adc[i] * smd_north_rgain[i]; // sout -> north for PHENIX -> sPHENIX
    smd_adc[i + 16] = smd_adc[i + 16] * smd_south_rgain[i]; // north -> south for PHENIX-> sPHENIX
  }
}

void ZdcMon::CompSmdPos() //computing position with weighted averages
{
  float w_ave[4]; // 0 -> north hor; 1 -> noth vert; 2 -> south hor; 3 -> south vert.
  float weights[4] = {0};
  memset(weights, 0, sizeof(weights)); // memset float works only for 0
  float w_sum[4];
  memset(w_sum, 0, sizeof(w_sum));

  // these constants convert the SMD channel number into real dimensions (cm's)
  const float hor_scale = 2.0 * 11.0 / 10.5 * sin(M_PI_4); // from gsl_math.h
  const float ver_scale = 1.5 * 11.0 / 10.5;
  float hor_offset = (hor_scale * 8 / 2.0) * (7.0 / 8.0);
  float ver_offset = (ver_scale * 7 / 2.0) * (6.0 / 7.0);

  for (int i = 0; i < 8; i++)
  {
    weights[0] += smd_adc[i]; //summing weights
    weights[2] += smd_adc[i + 16];
    w_sum[0] += (float)i * smd_adc[i]; //summing for the average
    w_sum[2] += ((float)i + 16.) * smd_adc[i + 16];
  }
  for (int i = 0; i < 7; i++)
  {
    weights[1] += smd_adc[i + 8];
    weights[3] += smd_adc[i + 24];
    w_sum[1] += ((float)i + 8.) * smd_adc[i + 8];
    w_sum[3] += ((float)i + 24.) * smd_adc[i + 24];
  }

  if ( weights[0] > 0.0 )
  {
    w_ave[0] = w_sum[0] / weights[0]; //average = sum / sumn of weights...
    smd_pos[0] = hor_scale * w_ave[0] - hor_offset;
  }
  else
  {
    smd_pos[0] = 0;
  }
  if ( weights[1] > 0.0 )
  {
    w_ave[1] = w_sum[1] / weights[1];
    smd_pos[1] = ver_scale * (w_ave[1] - 8.0) - ver_offset;
  }
  else
  {
    smd_pos[1] = 0;
  }

  if ( weights[2] > 0.0 )
  {
    w_ave[2] = w_sum[2] / weights[2];
    smd_pos[2] = hor_scale * (w_ave[2] - 16.0) - hor_offset;
  }
  else
  {
    smd_pos[2] = 0;
  }

  if ( weights[3] > 0.0 )
  {
    w_ave[3] = w_sum[3] / weights[3];
    smd_pos[3] = ver_scale * (w_ave[3] - 24.0) - ver_offset;
  }
  else
  {
    smd_pos[3] = 0;
  }
}

void ZdcMon::CompSumSmd() //compute 'digital' sum
{
  memset(smd_sum, 0, sizeof(smd_sum));

  for (int i = 0; i < 8; i++)
    {
      smd_sum[0] += smd_adc[i]; // north horizontal
      smd_sum[2] += smd_adc[i + 16]; // south horizontal
    }
  for (int i = 0; i < 7; i++)
    {
      smd_sum[1] += smd_adc[i + 8]; // north vertical
      smd_sum[3] += smd_adc[i + 24]; // south vertical
    }
}


