#ifndef CEMC_CEMCMON_H
#define CEMC_CEMCMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class CemcMon : public OnlMon
{
 public:
  CemcMon(const std::string &name = "CEMCMON");
  virtual ~CemcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *cemchist1 = nullptr;
  TH2 *cemchist2 = nullptr;
};

#endif /* CEMC_CEMCMON_H */
