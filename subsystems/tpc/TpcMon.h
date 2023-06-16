#ifndef TPC_TPCMON_H
#define TPC_TPCMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;
class TTree;

class TpcMon : public OnlMon
{
 public:
  TpcMon(const std::string &name);
  virtual ~TpcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int evtcnt = 0;
  int idummy = 0;

  static const int N_rBins = 4; //(one inner bin not filled, 2nd bin is R1, 3rd bin is R2, 4th bin is R3)
  const int N_thBins = 12; //(12 theta bins of uniform angle (360/12 = 30 degrees = TMath::Pi()/6 ~= 0.523 rad)
  const double rBin_edges[N_rBins+1] = {0.0, 0.256, 0.504, 0.752, 1.00}; //variable edges for radial dims

  TH1 *tpchist1 = nullptr;
  TH2 *tpchist2 = nullptr;
  
  TH2 *NorthSideADC = nullptr;
  TH2 *SouthSideADC = nullptr;

  TH2 *ADC_vs_SAMPLE = nullptr;

  TH1 *sample_size_hist = nullptr;
  TH1 *Check_Sum_Error = nullptr;
  TH1 *Check_Sums = nullptr;

  TH2 *MAXADC = nullptr;

  TH1 *RAWADC_1D_R1= nullptr;
  TH1 *MAXADC_1D_R1 = nullptr;

  TH1 *RAWADC_1D_R2= nullptr;
  TH1 *MAXADC_1D_R2 = nullptr;

  TH1 *RAWADC_1D_R3= nullptr;
  TH1 *MAXADC_1D_R3 = nullptr;

  int starting_BCO;
  int rollover_value;
  int current_BCOBIN;

  int serverid;

  void Locate(int id, float *rbin, float *thbin);
  int Index_from_Module(int sec_id, int fee_id);
  int Module_ID(int fee_id);
  int Max_Nine(int one, int two, int three, int four, int five, int six, int seven, int eight, int nine);
};

#endif /* TPC_TPCMON_H */
