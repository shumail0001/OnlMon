#ifndef INTT_MON_H
#define INTT_MON_H

#include <onlmon/OnlMon.h>

#include <string>

class Packet;
class TH1;

class InttMon : public OnlMon
{
 public:
  InttMon(const std::string& name);
  virtual ~InttMon();

  int Init();
  int BeginRun(const int);
  int process_event(Event*);
  int Reset();

  int MiscDebug();

 private:
  static constexpr int NCHIPS = 26;
  static constexpr int NFEES = 14;
  static constexpr int NBCOS = 128;
  Packet** plist{nullptr};
  TH1* EvtHist{nullptr};
  TH1* HitHist{nullptr};
  TH1* BcoHist{nullptr};
};

#endif
