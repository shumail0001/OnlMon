#ifndef MVTX_MVTXMON_H
#define MVTX_MVTXMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class MvtxMon : public OnlMon
{
 public:
  MvtxMon(const std::string &name = "MVTXMON");
  virtual ~MvtxMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *mvtxhist1 = nullptr;
  TH2 *mvtxhist2 = nullptr;
};

#endif /* MVTX_MVTXMON_H */
