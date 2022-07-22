#ifndef MBDLL1_MBDLL1MON_H
#define MBDLL1_MBDLL1MON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class Mbdll1Mon : public OnlMon
{
 public:
  Mbdll1Mon(const std::string &name = "MBDLL1MON");
  virtual ~Mbdll1Mon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *mbdll1hist1 = nullptr;
  TH2 *mbdll1hist2 = nullptr;
};

#endif /* MBDLL1_MBDLL1MON_H */
