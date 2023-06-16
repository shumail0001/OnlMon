#ifndef __LL1HEADER_H__
#define __LL1HEADER_H__

#include "TObject.h"
#include <iostream>
#include <string>

static const int NRMOD = 4;
static const int NMODBYXMIT[] = {1};

static const int NXMIT = sizeof(NMODBYXMIT)/sizeof(*NMODBYXMIT);
static const int PACKET[NXMIT] = {14002};

static const int NCHANPERMOD = 64;
static const int NRSAM = 20;
static const int NPEDSAM = 3;

static const int NCH = 52;
static const int NTRIGWORDS = 8;
static const int NCHARGESUM= 8;
static const int NHITCHANNEL= 8;
static const int NTIMESUM= 4;
static const int NADCSH= 13;

class LL1HEADER
{

 public:
  int runnumber;
  int evtnr;
  int clock;

  int evenchecksum;
  int calcevenchecksum;
  int oddchecksum;
  int calcoddchecksum;

  int femslot[NRMOD];
  int femevtnr[NRMOD];
  int femclock[NRMOD];

  int adc[NRMOD*NCHANPERMOD][NRSAM];
  int channel[NCH][NRSAM];
  int triggerwords[NTRIGWORDS][NRSAM];
  float mean[NRMOD*NCHANPERMOD];
  float rms[NRMOD*NCHANPERMOD];
  int minadc[NRMOD*NCHANPERMOD];
  int maxadc[NRMOD*NCHANPERMOD];
  float pedestal[NRMOD*NCHANPERMOD];
  float pedsig[NRMOD*NCHANPERMOD];
  float peak[NRMOD*NCHANPERMOD];
  float time[NRMOD*NCHANPERMOD];
  float integral[NRMOD*NCHANPERMOD];
  float integral2[NRMOD*NCHANPERMOD];

  int nhit_n1[NRSAM];
  int nhit_n2[NRSAM];
  int nhit_s1[NRSAM];
  int nhit_s2[NRSAM];
  int nhit_n[NRSAM];
  int nhit_s[NRSAM];
  int chargesum_s1[NRSAM];
  int chargesum_s2[NRSAM];
  int chargesum_n1[NRSAM];
  int chargesum_n2[NRSAM];
  int chargesum_s[NRSAM];
  int chargesum_n[NRSAM];
  int timesum_s1[NRSAM];
  int timesum_s2[NRSAM];
  int timesum_n1[NRSAM];
  int timesum_n2[NRSAM];
  int timesum_s[NRSAM];
  int timesum_n[NRSAM];

  int idxsample;
  int idxhitn;
  int idxhits;

  int summary[NXMIT];
  
  LL1HEADER() {
    runnumber = 0;

  };
  
  ~LL1HEADER() {
  }; 

};

#endif /* __LL1HEADER_H__ */
