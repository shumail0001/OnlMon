#ifndef MYMON_H__
#define MYMON_H__

#include <onlmon/OnlMon.h>
#include <string>

class Event;
class OnlMonDB;
class TH1;
class TH2;

class MyMon: public OnlMon
{
 public:
  MyMon(const char *name = "MYMON");
  virtual ~MyMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int DBVarInit();
  int evtcnt;
  int idummy;
  OnlMonDB *dbvars;
  TH1 *myhist1;
  TH2 *myhist2;

};

#endif /* MYMON_H__ */

