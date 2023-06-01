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

  TH1 *sample_size_hist = nullptr;
  TH1 *Check_Sum_Error = nullptr;
  TH1 *Check_Sums = nullptr;

  int serverid = MonitorServerId();

  void Locate(int id, float *rbin, float *thbin);
  int Index_from_Module(int sec_id, int fee_id);
};

#endif /* TPC_TPCMON_H */
