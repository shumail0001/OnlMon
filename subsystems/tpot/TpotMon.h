#ifndef TPOT_TPOTMON_H
#define TPOT_TPOTMON_H

#include <onlmon/OnlMon.h>

#include <array>
#include <memory>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class TpotMon : public OnlMon
{
 public:
  TpotMon(const std::string &name = "TPOTMON");
  ~TpotMon() override = default;

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  std::unique_ptr<OnlMonDB> dbvars;

  /// total number of phi detectors  
  static constexpr int n_detectors_phi = 8;

  /// total number of phi detectors  
  static constexpr int n_detectors_z = 8;

  /// total number of detectors  
  static constexpr int n_detectors = 16;

  /// number of resist sector in each detector
  static constexpr int n_resist = 4;
  
  ///@name on/off histograms
  //@{
  TH2* m_hv_onoff_phi = nullptr;
  TH2* m_hv_onoff_z = nullptr;
  
  TH2* m_fee_onoff_phi = nullptr;
  TH2* m_fee_onoff_z = nullptr;
  //@}
  
  ///@name raw hits histograms
  //@{
  /// adc counts vs sample id in each detector
  std::array<TH1*, n_detectors> m_adc_vs_sample = {{nullptr}};

  /// total charge per hit in each detector
  std::array<TH1*, n_detectors> m_hit_charge= {{nullptr}};
  
  /// number of hits per event in each detector
  std::array<TH1*, n_detectors> m_hit_multiplicity= {{nullptr}};

  /// total number of hits per channel in each detector
  std::array<TH1*, n_detectors> m_hit_vs_channel = {{nullptr}};

  //@}
  
  ///@name cluster histograms
  //@{
  /// cluster size distributin in each detector
  std::array<TH1*, n_detectors> m_cluster_size = {{nullptr}};
  
  /// charge per cluster in each detector
  std::array<TH1*, n_detectors> m_cluster_charge = {{nullptr}};

  /// number of clusters per event in each detector
  std::array<TH1*, n_detectors> m_cluster_multiplicity= {{nullptr}};
  //@}
};

#endif /* TPOT_TPOTMON_H */
