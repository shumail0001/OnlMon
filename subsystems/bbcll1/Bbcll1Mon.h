#ifndef BBCLL1_BBCLL1MON_H
#define BBCLL1_BBCLL1MON_H

#include <onlmon/OnlMon.h>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class Bbcll1Mon : public OnlMon
{
 public:
  Bbcll1Mon(const std::string &name = "BBCLL1MON");
  virtual ~Bbcll1Mon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *bbcll1hist1 = nullptr;
  TH2 *bbcll1hist2 = nullptr;
};

#endif /* BBCLL1_BBCLL1MON_H */
