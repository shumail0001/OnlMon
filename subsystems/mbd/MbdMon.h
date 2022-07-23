#ifndef MBD_MBDMON_H
#define MBD_MBDMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class MbdMon : public OnlMon
{
 public:
  MbdMon(const std::string &name = "MBDMON");
  virtual ~MbdMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *mbdhist1 = nullptr;
  TH2 *mbdhist2 = nullptr;
};

#endif /* MBD_MBDMON_H */
