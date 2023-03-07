#ifndef TPOT_TPOTMON_H
#define TPOT_TPOTMON_H

#include "TpotDefs.h"

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
  TpotMon(const std::string &name);
  ~TpotMon() override = default;

  int process_event(Event *evt) override;
  int Init() override;
  int BeginRun(const int runno) override;
  int Reset() override;

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  std::unique_ptr<OnlMonDB> dbvars;
  
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
  std::array<TH1*, TpotDefs::n_detectors> m_adc_vs_sample = {{nullptr}};

  /// total charge per hit in each detector
  std::array<TH1*, TpotDefs::n_detectors> m_hit_charge= {{nullptr}};
  
  /// number of hits per event in each detector
  std::array<TH1*, TpotDefs::n_detectors> m_hit_multiplicity= {{nullptr}};

  /// total number of hits per channel in each detector
  std::array<TH1*, TpotDefs::n_detectors> m_hit_vs_channel = {{nullptr}};

  //@}
  
};

#endif /* TPOT_TPOTMON_H */
