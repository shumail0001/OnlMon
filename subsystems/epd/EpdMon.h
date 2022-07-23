#ifndef EPD_EPDMON_H
#define EPD_EPDMON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class EpdMon : public OnlMon
{
 public:
  EpdMon(const std::string &name = "EPDMON");
  virtual ~EpdMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *epdhist1 = nullptr;
  TH2 *epdhist2 = nullptr;
};

#endif /* EPD_EPDMON_H */
