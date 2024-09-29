#ifndef __LL1HEADER_H__
#define __LL1HEADER_H__

#include <TObject.h>

#include <iostream>
#include <string>

static const int NXMIT = 18;
static const Int_t PACKET[NXMIT] = {13001, 13002, 13010, 13011, 13012, 13013, 13014, 13015, 13016, 13017, 13018, 13019, 13020, 13021, 13022, 13023, 13024, 13025};

static const int NCHANPERMOD = 64;
static const int NRSAM = 5;
static const int NPEDSAM = 3;

static const int NCH = 52;
static const int NRMOD = 4;
static const int NTRIGWORDS = 8;
static const int NCHARGESUM= 8;
static const int NHITCHANNEL= 8;
static const int NTIMESUM= 4;
static const int NADCSH= 13;
static const int nthresholds = 13;
class LL1HEADER
{

 public:
  int runnumber=0;
  int evtnr=0;
  int clock=0;
  int nsamples=NRSAM;
  int channels=0;
  uint16_t monitor = 0;
  
  int evenchecksum=0;
  int calcevenchecksum=0;
  int oddchecksum=0;
  int calcoddchecksum=0;

  int femslot[NRMOD]={0};
  int femevtnr[NRMOD]={0};
  int femclock[NRMOD]={0};

  int adc[NRMOD*NCHANPERMOD][NRSAM]={0};
  int channel[NCH][NRSAM]={0};
  int triggerwords[NTRIGWORDS][NRSAM]={0};
  float mean[NRMOD*NCHANPERMOD]={0.};
  float rms[NRMOD*NCHANPERMOD]={0.};
  int minadc[NRMOD*NCHANPERMOD]={0};
  int maxadc[NRMOD*NCHANPERMOD]={0};
  float pedestal[NRMOD*NCHANPERMOD]={0.};
  float pedsig[NRMOD*NCHANPERMOD]={0.};
  float peak[NRMOD*NCHANPERMOD]={0.};
  float time[NRMOD*NCHANPERMOD]={0.};
  float integral[NRMOD*NCHANPERMOD]={0.};
  float integral2[NRMOD*NCHANPERMOD]={0.};

  int nhit_n1[NRSAM]={0};
  int nhit_n2[NRSAM]={0};
  int nhit_s1[NRSAM]={0};
  int nhit_s2[NRSAM]={0};
  int nhit_n[NRSAM]={0};
  int nhit_s[NRSAM]={0};
  int chargesum_s1[NRSAM]={0};
  int chargesum_s2[NRSAM]={0};
  int chargesum_n1[NRSAM]={0};
  int chargesum_n2[NRSAM]={0};
  int chargesum_s[NRSAM]={0};
  int chargesum_n[NRSAM]={0};
  int timesum_s1[NRSAM]={0};
  int timesum_s2[NRSAM]={0};
  int timesum_n1[NRSAM]={0};
  int timesum_n2[NRSAM]={0};
  int timesum_s[NRSAM]={0};
  int timesum_n[NRSAM]={0};

  int hit_format_jet = 0;
  int jet_threshold[4]={0};
  int photon_threshold[4]={0};
  int mbd_threshold[5]={0};

  int idxsample={0};
  int idxhitn={0};
  int idxhits={0};

  int jet_input[12][32]={0};
  int jet_output[9][32]={0};

  int emcal_2x2_map[48][128]={0};
  int emcal_8x8_map[12][32] = {0};;

  int emcal_sample[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  int jet_sample[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


  int summary[NXMIT]={0};
  
  LL1HEADER() {};
  
  ~LL1HEADER() {}; 

};

#endif /* __LL1HEADER_H__ */
