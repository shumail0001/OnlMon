#ifndef CEMC_CEMCMON_H
#define CEMC_CEMCMON_H


#include <onlmon/OnlMon.h>

#include <vector>

class CaloWaveformProcessing;
class TowerInfoContainerv1;
class Event;
class OnlMonDB;
class TH1;
class TH2;
class Packet;
class runningMean;

class CemcMon : public OnlMon
{
 public:
  CemcMon(const std::string &name);
  virtual ~CemcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  std::vector<float> getSignal(Packet *p, const int channel);
  std::vector<float> anaWaveform(Packet *p, const int channel);

  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH2 *cemc_occupancy = nullptr;
  TH2 *cemc_runningmean = nullptr;
  TH1 *cemc_signal = nullptr;

  const int Nsector = 32;
  const int Ntower = 1536*4;
  const int packetlow = 6001;
  const int packethigh = 6024;
  TH2* h2_hcal_hits = nullptr;
  TH1* h_waveform_twrAvg = nullptr;
  TH1* h_waveform_time = nullptr;
  TH1* h_waveform_pedestal = nullptr;
  TH2* h2_hcal_rm = nullptr;
  TH2* h2_hcal_mean = nullptr;
  TH1* h_sectorAvg_total = nullptr;
  TH1* h_event = nullptr;
  TH1* h_rm_sectorAvg[100] = {nullptr};

  std::vector<runningMean*> rm_vector_twr;
  std::vector<runningMean*> rm_vector_sectAvg;

  std::string runtypestr = "Unknown";
  std::string id_string;

  CaloWaveformProcessing* WaveformProcessing = nullptr;
  TowerInfoContainerv1* CaloInfoContainer = nullptr;  // for using encode_key


  std::vector<runningMean*> rm_vector; 

};

#endif /* CEMC_CEMCMON_H */
