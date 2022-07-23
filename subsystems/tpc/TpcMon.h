#ifndef TPC_TPCMON_H
#define TPC_TPCMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class TpcMon : public OnlMon
{
 public:
  TpcMon(const std::string &name = "TPCMON");
  virtual ~TpcMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *tpchist1 = nullptr;
  TH2 *tpchist2 = nullptr;
};

#endif /* TPC_TPCMON_H */
