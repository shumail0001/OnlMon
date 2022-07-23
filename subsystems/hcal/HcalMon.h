#ifndef HCAL_HCALMON_H
#define HCAL_HCALMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class HcalMon : public OnlMon
{
 public:
  HcalMon(const std::string &name = "HCALMON");
  virtual ~HcalMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *hcalhist1 = nullptr;
  TH2 *hcalhist2 = nullptr;
};

#endif /* HCAL_HCALMON_H */
