#ifndef SPIN_SPINMON_H
#define SPIN_SPINMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;
class Packet;
class eventReceiverClient;


class SpinMon : public OnlMon
{
 public:
  SpinMon(const std::string &name = "SPINMON");
  virtual ~SpinMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

  static const int NBUNCHES = 100; // OG is 120

  private:

    // Packet *p = nullptr;
    Packet *pBlueSpin = nullptr;
    Packet *pYellSpin = nullptr;

    



    int blueSpinPattern[NBUNCHES] = {0};
    int yellSpinPattern[NBUNCHES] = {0};

 protected:
  int evtcnt = 0;

  eventReceiverClient *erc = {nullptr};

  const int BLUE = 0;
  const int YELLOW = 1;

  const int SPIN_UP = 0;
  const int SPIN_DOWN = 1;
  const int SPIN_UNPOL = 2;

  const int packet_BLUESPIN = 14902;
  const int packet_YELLSPIN = 14903;
  const int packetid_GL1 = 14001;

  static const int NTRIG = 16;

  //TH2 *spin_patterns[2] = {nullptr};

  TH2 *spin_patternBlueUp = nullptr;
  TH2 *spin_patternBlueDown = nullptr;
  TH2 *spin_patternBlueUnpol = nullptr;

  TH2 *spin_patternYellowUp = nullptr;
  TH2 *spin_patternYellowDown = nullptr;
  TH2 *spin_patternYellowUnpol = nullptr;

  TH1 *gl1_counter[NTRIG] = {nullptr};
  
};
    
#endif /* SPIN_SPINMON_H */
