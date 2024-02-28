#ifndef ZDC_ZDCMON_H
#define ZDC_ZDCMON_H

#include <onlmon/OnlMon.h>

#include <onlmon/OnlMon.h>
#include <cmath>
#include <vector>

class CaloWaveformFitting;
class TowerInfoContainer;
class Event;
class TH1;
class TH2;
class Packet;

class ZdcMon : public OnlMon
{
 public:
  ZdcMon(const std::string &name = "ZDCMON");
  virtual ~ZdcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  std::vector<float> anaWaveformFast(Packet *p, const int channel);
  CaloWaveformFitting *WaveformProcessingFast = nullptr;

  int evtcnt = 0;
  int idummy = 0;
  TH1 *zdc_adc_north = nullptr;
  TH1 *zdc_adc_south = nullptr;
  // smd
  // north smd
  TH1 *smd_hor_north = nullptr;
  TH1 *smd_ver_north = nullptr;
  TH1 *smd_sum_hor_north = nullptr;
  TH1 *smd_sum_ver_north = nullptr;
  // south smd 
  TH1 *smd_hor_south;
  TH1 *smd_ver_south = nullptr;
  TH1 *smd_sum_hor_south = nullptr;
  TH1 *smd_sum_ver_south = nullptr;
  // smd values
  TH1 *smd_value = nullptr;
  TH1 *smd_value_good = nullptr;
  TH1 *smd_value_small = nullptr;

  void GetCalConst();
  void CompSmdAdc();
  void CompSmdPos();
  void CompSumSmd();
  void CompZdcAdc();



  float smd_adc[32];
  float zdc_adc[8];
  float smd_sum[4]; 
  float smd_pos[4];

  float pedestal[40];
  float zdc_ped[8];
  float smd_ped[32];
  float gain[32];
  float smd_south_rgain[16];
  float smd_north_rgain[16];
  float overflow0[40];
  float overflow1[40];

};

#endif /* ZDC_ZDCMON_H */
