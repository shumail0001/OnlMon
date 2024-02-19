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
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");

  // Create hitograms
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

    float totalzdcsouthsignal = 0.;
    float totalzdcnorthsignal = 0.;
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
          
        int mod = c%2;
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

    // get ped_zdc for north and south inverted from PHENIX to sPHENIX
    bool ped_zdc_north = (zdc_adc[0] > 70.); //60 in 200GeV Cu or Au runs
    bool ped_zdc_south = (zdc_adc[4] > 70.); //70 in 200GeV Cu or Au runs



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

void ZDCMon::CompSmdAdc() //substacting pedestals and multiplying with gains
{
  for (int i = 16; i < 48; i++) // 8->16
    {
      float temp = p->iValue(i) - smd_ped[i - 16]; // 8->16
      //if (temp < 0.0 )
      //  temp = 0.0;
      smd_adc[i - 16] = temp; // 8-> 16
    }

  for (int i = 0; i < 15; i++) // last one is reserved for analogue sum
    {
      // multiply SMD channels with their gain factors
      // to get the absolute ADC values in the same units
      //rgains come from CompSmdAdc()
      smd_adc[i] = smd_adc[i] * smd_north_rgain[i]; // sout -> north for PHENIX -> sPHENIX
      smd_adc[i + 16] = smd_adc[i + 16] * smd_south_rgain[i]; // north -> south for PHENIX-> sPHENIX
    }
}

void ZDCMon::CompSmdPos() //computing position with weighted averages
{
  float w_ave[4]; // 1 -> north hor; 2 -> noth vert; 3 -> south hor; 4 -> south vert.
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

void ZDCMon::CompSumSmd() //compute 'digital' sum
{
  memset(smd_sum, 0, sizeof(smd_sum));

  for (int i = 0; i < 8; i++)
    {
      smd_sum[0] += smd_adc[i];
      smd_sum[2] += smd_adc[i + 16];
    }
  for (int i = 0; i < 7; i++)
    {
      smd_sum[1] += smd_adc[i + 8];
      smd_sum[3] += smd_adc[i + 24];
    }
}


