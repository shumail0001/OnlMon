#ifndef HCAL_HCALMON_H
#define HCAL_HCALMON_H

#include <calobase/TowerInfoContainerv1.h>
#include <onlmon/OnlMon.h>

#include <vector>

class CaloWaveformProcessing;
class Event;
class OnlMonDB;
class TH1;
class TH2;
class Packet;
class runningMean;

class HcalMon : public OnlMon
{
 public:
  HcalMon(const std::string& name = "HCALMON");
  virtual ~HcalMon();

  int process_event(Event* evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();
  std::vector<float> getSignal(Packet* p, const int channel);
  std::vector<float> anaWaveform(Packet* p, const int channel);

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  const int Nsector = 32;
  const int Ntower = 1536;
  const int packetlow = 8001;
  const int packethigh = 8024;
  OnlMonDB* dbvars = nullptr;
  // TH1 *hcalhist1 = nullptr;
  // TH2 *hcalhist2 = nullptr;
  TH2* h2_hcal_hits = nullptr;
  TH1* h_waveform_twrAvg = nullptr;
  TH1* h_waveform_time = nullptr;
  TH1* h_waveform_pedestal = nullptr;
  TH2* h2_hcal_rm = nullptr;
  TH2* h2_hcal_mean = nullptr;
  TH1* h_sectorAvg_total = nullptr;
  TH1* h_event = nullptr;
  TH1* h_rm_sectorAvg[100] = {nullptr};

  CaloWaveformProcessing* WaveformProcessing = nullptr;
  TowerInfoContainerv1* CaloInfoContainer = nullptr;  // for using encode_key

  std::vector<runningMean*> rm_vector_sectAvg;
  std::vector<runningMean*> rm_vector_twr;
};

#endif /* HCAL_HCALMON_H */
