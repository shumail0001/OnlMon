#ifndef SPIN_SPINMON_H
#define SPIN_SPINMON_H

#include <onlmon/OnlMon.h>

class Event;
class TH1;
class TH2;
class Packet;
class eventReceiverClient;
class XingShiftCal;


class SpinMon : public OnlMon
{
 public:
  SpinMon(const std::string &name = "SPINMON");
  virtual ~SpinMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

  static const int NTRIG = 16;
  static const int NBUNCHES = 120; // OG is 120

  private:

    // Packet *p = nullptr;
    Packet *pBlueSpin = nullptr;
    Packet *pYellSpin = nullptr;
    Packet *pBluePol = nullptr;
    Packet *pYellPol = nullptr;
    Packet *pBlueAsym = nullptr;
    Packet *pYellAsym = nullptr;
    Packet *pBlueIntPattern = nullptr;
    Packet *pYellIntPattern = nullptr;
    Packet *pBluePolPattern = nullptr;
    Packet *pYellPolPattern = nullptr;

    bool success = 0;
    // default xingshift
    int defaultxingshift = 5;
    // for additional xingshift
    int xingshift = 5;

    uint64_t scalercounts[NTRIG][NBUNCHES]{};

    int blueSpinPattern[NBUNCHES] = {0};
    int yellSpinPattern[NBUNCHES] = {0};

 protected:
  int evtcnt = 0;

  XingShiftCal *XingShift = nullptr;

  eventReceiverClient *erc = {nullptr};

  const int BLUE = 0;
  const int YELLOW = 1;

  const int SPIN_UP = 0;
  const int SPIN_DOWN = 1;
  const int SPIN_UNPOL = 2;

  const int packet_BLUESPIN = 14902;
  const int packet_YELLSPIN = 14903;
  //const int packet_BLUEPOL = 14905;
  //const int packet_YELLPOL = 14906;
  //const int packet_BLUEASYM = 14907;
  //const int packet_YELLASYM = 14908;
  //const int packet_BLUEINTPATTERN = 14910;
  //const int packet_YELLINTPATTERN = 14911;
  //const int packet_BLUEPOLPATTERN = 14912;
  //const int packet_YELLPOLPATTERN = 14913;
  const int packetid_GL1 = 14001;


  //TH2 *spin_patterns[2] = {nullptr};
  TH1 *hspinpatternBlue = nullptr;
  TH1 *hspinpatternYellow = nullptr;

  TH2 *pCspin_patternBlueUp = nullptr;
  TH2 *pCspin_patternBlueDown = nullptr;
  TH2 *pCspin_patternBlueUnpol = nullptr;

  TH2 *pCspin_patternYellowUp = nullptr;
  TH2 *pCspin_patternYellowDown = nullptr;
  TH2 *pCspin_patternYellowUnpol = nullptr;
  
  TH2 *spin_patternBlueUp = nullptr;
  TH2 *spin_patternBlueDown = nullptr;
  TH2 *spin_patternBlueUnpol = nullptr;

  TH2 *spin_patternYellowUp = nullptr;
  TH2 *spin_patternYellowDown = nullptr;
  TH2 *spin_patternYellowUnpol = nullptr;

  TH1 *hpolBlue = nullptr;
  TH1 *hpolYellow = nullptr;

  TH1 *hxingshift = nullptr;

  TH1 *gl1_counter[NTRIG] = {nullptr};

  //uint64_t scalercounts[NTRIG][NBUNCHES]{};
  
};
    
#endif /* SPIN_SPINMON_H */
