#ifndef ONLMONSERVER_ONLMON_H
#define ONLMONSERVER_ONLMON_H

#include "OnlMonBase.h"

#include <iostream>
#include <set>
#include <string>

class Event;
class OnlMonServer;
class TH1;

class OnlMon : public OnlMonBase
{
 public:
  OnlMon(const std::string &name = "NONE");
  ~OnlMon() override {}

  enum
  {
    ACTIVE = -1,
    OK = 0,
    WARNING = 1,
    ERROR = 2
  };
  virtual int process_event_common(Event *evt);
  virtual int process_event(Event *evt);
  virtual int InitCommon(OnlMonServer *se);
  virtual int Init() { return 0; }
  virtual int Reset();
  virtual void identify(std::ostream &out = std::cout) const;
  virtual int BeginRunCommon(const int runno, OnlMonServer *se);
  virtual int BeginRun(const int /* runno */) { return 0; }
  virtual int EndRun(const int /* runno */) { return 0; }
  virtual void AddTrigger(const std::string &name);
  virtual void AddLiveTrigger(const std::string &name);
  virtual void SetStatus(const int newstatus);
  virtual int ResetEvent() { return 0; }
  virtual void SetMonitorServerId(unsigned int i);

 protected:
  std::set<std::string> TriggerList;      // trigger selection send to the et pool
  std::set<std::string> LiveTriggerList;  // triggers filtered in process_event according to live bit
  unsigned int livetrigmask;
  int status;
  unsigned int m_MonitorServerId = 0;
  TH1 *m_LocalFrameWorkVars = nullptr;
};

#endif /* ONLMONSERVER_ONLMON_H */
