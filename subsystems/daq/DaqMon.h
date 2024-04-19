#ifndef DAQ_DAQMON_H
#define DAQ_DAQMON_H

#include <onlmon/OnlMon.h>
#include <TH2D.h>

#include <vector>
class Event;
class TH1;
class TH2;
class runningMean;
class eventReceiverClient;
class Packet;

class DaqMon : public OnlMon
{
 public:
  DaqMon(const std::string &name);
  virtual ~DaqMon();

  int process_event(Event* evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();
  int CaloPacketMap(int pnum);
          
  uint32_t previousdiff = 0;
  uint32_t clockdiff = 0;


 protected:
  int evtcnt = 0;
  int gevtcnt = 0;
  int binindex = 0;
  int previndex = 0;

  int nEventsCapture=1000000;
  int idummy = 0;
  int packetlow = 6001;
  int packethigh = 12001;
  int packet_mbd_low = 1001;
  int packet_mbd_high = 1002;
  int packet_emcal_low = 6001;
  int packet_emcal_high = 6128;
  int packet_ihcal_low = 7001;
  int packet_ihcal_high = 7008;
  int packet_ohcal_low = 8001;
  int packet_ohcal_high = 8008;
  int packet_sepd_low = 9001;
  int packet_sepd_high = 9006;
  int packet_zdc = 12001;
  
  int npackets_emcal = 128;
  int npackets_ihcal = 8;
  int npackets_ohcal = 8;
  int npackets_sepd= 6;
  int npackets_mbd= 2;
  int npackets_zdc= 1;

  TH1 *daqhist1 = nullptr;
  TH2 *daqhist2 = nullptr;
  TH2D *h_gl1_clock_diff= nullptr;
  TH2D *h_gl1_clock_diff_capture= nullptr;
  eventReceiverClient *erc = {nullptr};
};

#endif /* DAQ_DAQMON_H */
