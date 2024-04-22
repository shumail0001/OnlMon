#ifndef LOCALPOL_LOCALPOLMON_H
#define LOCALPOL_LOCALPOLMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;
class Packet;
class eventReceiverClient;


class LocalPolMon : public OnlMon
{
 public:
  LocalPolMon(const std::string &name = "LOCALPOLMON");
  virtual ~LocalPolMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  int evtcnt = 0;
  int packetid = 12001;

  TH1 *h_example = nullptr;
  TH1 *h_example2 = nullptr;

  
};
    
#endif /* LOCALPOL_LOCALPOLMON_H */
