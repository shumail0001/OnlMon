#ifndef TPOT_TPOTMON_H
#define TPOT_TPOTMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class TpotMon : public OnlMon
{
 public:
  TpotMon(const std::string &name = "TPOTMON");
  virtual ~TpotMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *tpothist1 = nullptr;
  TH2 *tpothist2 = nullptr;
};

#endif /* TPOT_TPOTMON_H */
